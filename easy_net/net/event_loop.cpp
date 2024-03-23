#include "event_loop.h"
#include <csignal>
#include <cstdio>
#include <mutex>
#include <sstream>
#include <sys/epoll.h>
#include <thread>
#include <vector>

#include "io_event.h"
#include "non_copyable.h"
#include "notify.h"
#include "poller.h"
#include "spdlog/spdlog.h"
#include "timer_miniheap.h"

namespace EasyNet {
class IgnoreSigPipe {
 public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};
IgnoreSigPipe initObj;
} // namespace EasyNet

using namespace EasyNet;

thread_local EventLoop *t_loopInThread = nullptr;

EventLoop::EventLoop() : m_poller(Poller::CreatePoller(poller_type_e::TYPE_EPOLL, this)),
                         m_pending_func(false),
                         m_quit(false),
                         m_looping(false),
                         m_threadid(std::this_thread::get_id()) {
    m_notifyer = make_unique<Notify>(this);
    m_timer_queue = make_unique<MiniHeapTimer>();

    std::stringstream ss;
    ss << m_threadid;
    if (t_loopInThread) {
        spdlog::critical("Another Loop bind in this thread:{}", ss.str());
    } else {
        t_loopInThread = this;
        spdlog::info("EventLoop bind in this thread:{}", ss.str());
    }
}

void EventLoop::Loop() {
    if (m_looping) {
        return;
    }

    m_quit = false;

    while (!m_quit) {
        auto tm = m_timer_queue->find_timer();
        m_ready_events.clear();
        m_poller->Polling(tm, m_ready_events);

        // 1,处理到期事件
        m_timer_queue->handle_expired_timer();

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
        _do_pending_functions();
    }

    m_looping = false;
}

bool EventLoop::IsThreadInLoop() {
    return m_threadid == std::this_thread::get_id();
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

void EventLoop::_do_pending_functions() {
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