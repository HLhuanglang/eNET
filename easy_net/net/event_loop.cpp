#include "event_loop.h"

#include <sys/epoll.h>

#include <csignal>
#include <cstdio>
#include <mutex>
#include <vector>

#include "io_event.h"
#include "log.h"
#include "non_copyable.h"
#include "notify.h"
#include "poller.h"
#include "thread.h"
#include "timer.h"

namespace EasyNet {
class IgnoreSigPipe {
 public:
    static void log(int sig) {
        LOG_DEBUG("recive sig={}", sig);
    }
    IgnoreSigPipe() {
        ::signal(SIGPIPE, log);
    }
};
IgnoreSigPipe initObj;
}  // namespace EasyNet

using namespace EasyNet;

thread_local EventLoop *t_loopInThread = nullptr;

EventLoop::EventLoop(std::string name) : m_poller(Poller::CreatePoller(poller_type_e::TYPE_EPOLL, this)),
                                         m_pending_func(false),
                                         m_quit(false),
                                         m_looping(false),
                                         m_threadid(GetCurrentThreadId()) {
    m_notifyer = make_unique<Notify>(this);
    m_timer_queue = make_unique<MiniHeapTimer>();

    auto id = GetCurrentThreadId();
    if (t_loopInThread) {
        LOG_ERROR("Another Loop bind in this thread:{}", id);
    } else {
        t_loopInThread = this;
        m_name = name + "-" + std::to_string(id);
        LOG_INFO("{} bind in this thread:{}", m_name, id);
    }
}

void EventLoop::Loop() {
    if (m_looping) {
        return;
    }

    m_quit = false;

    while (!m_quit) {
        auto tm = m_timer_queue->GetNextExpiredTime();
        m_ready_events.clear();
        m_poller->Polling(tm, m_ready_events);

        // 1,处理到期事件
        m_timer_queue->HandleExpiredTimer();

        // 2,处理当前处于活动状态的fd上的事件
        for (auto &it : m_ready_events) {
            it->DispatchEvent();
        }

        // 3,处理外部线程注入的回调函数队列(eg：定时任务)
        // 对于polling函数来说
        // 如果设置超时时间为0，那么在没有任何网络IO时间和其他任务处理的情况下，这些工作线程实际上会空转，白白地浪费cpu时间片。
        // 如果设置的超时时间大于0，在没有网络IO时间的情况，epoll_wait/poll/select仍然要挂起指定时间才能返回
        // 会影响其他任务不能及时处理
        // 如何解决呢？
        // 我们采取如下方法来解决该问题，以linux为例，不管epoll_fd上有没有文件描述符fd，我们都给它绑定一个默认的fd，这个fd被称为唤醒fd。
        // 当我们需要处理其他任务的时候，向这个唤醒fd上随便写入1个字节的，这样这个fd立即就变成可读的了，epoll_wait() / poll() / select() 函数立即被唤醒，并返回,接下来马上就能执行_do_pending_functions()，其他任务得到处理。
        DoPendiongFunc();
    }

    m_looping = false;
}

bool EventLoop::IsThreadInLoop() {
    return m_threadid == GetCurrentThreadId();
}

void EventLoop::RunInLoop(const pending_func_t &cb) {
    if (IsThreadInLoop()) {
        cb();
    } else {
        QueueInLoop(std::move(cb));
    }
}

void EventLoop::QueueInLoop(const pending_func_t &cb) {
    {
        std::lock_guard<std::mutex> lgmtx(m_mtx);
        m_pending_func_list.push_back(cb);
    }

    if (!IsThreadInLoop() || m_pending_func) {
        m_notifyer->WakeUp();
    }
}

void EventLoop::Quit() {
    // event_loop一定是卡在loop中的while循环
    m_quit = true;
    if (!IsThreadInLoop()) {
        m_notifyer->WakeUp();
    }
}

TimerID EventLoop::TimerAfter(const TimerCallBack &cb, int interval) {
    return m_timer_queue->AddTimer(interval, TimerType::E_AFTER, cb);
}

TimerID EventLoop::TimerEvery(const TimerCallBack &cb, int interval) {
    return m_timer_queue->AddTimer(interval, TimerType::E_EVERY, cb);
}

void EventLoop::CancelTimer(TimerID id) {
    m_timer_queue->CancelTimer(id);
}

void EventLoop::DoPendiongFunc() {
    std::vector<pending_func_t> functors;
    m_pending_func = true;
    {
        // fixme：锁住
        functors.swap(m_pending_func_list);
    }
    for (auto &fn : functors) {
        fn();
    }
    m_pending_func = false;
}