#include "event_loop.h"
#include <cstdio>
#include <sys/epoll.h>
#include <vector>

#include "fd_event.h"

event_loop::event_loop() : m_poller(create_poller(poller_type_e::TYPE_EPOLL, this)), m_pending_func(false), m_notifyer(this) {
}

void event_loop::loop() {
    m_poller->polling(10000, m_ready_events); // fixme：超时时间从定时器中获取
    m_quit = false;
    while (!m_quit) {
        // 1,处理当前处于活动状态的fd上的事件
        for (auto &it : m_ready_events) {
            it->handle_event();
        }
        // 2,处理外部线程注入的回调函数队列(eg：定时任务)
        // 对于polling函数来说
        // 如果设置超时时间为0，那么在没有任何网络IO时间和其他任务处理的情况下，这些工作线程实际上会空转，白白地浪费cpu时间片。
        // 如果设置的超时时间大于0，在没有网络IO时间的情况，epoll_wait/poll/select仍然要挂起指定时间才能返回
        // 会影响其他任务不能及时处理
        // 如何解决呢？
        // 我们采取如下方法来解决该问题，以linux为例，不管epoll_fd上有没有文件描述符fd，我们都给它绑定一个默认的fd，这个fd被称为唤醒fd。
        // 当我们需要处理其他任务的时候，向这个唤醒fd上随便写入1个字节的，这样这个fd立即就变成可读的了，epoll_wait() / poll() / select() 函数立即被唤醒，并返回,接下来马上就能执行_do_pending_functions()，其他任务得到处理。
        _do_pending_functions();
    }
}

void event_loop::_do_pending_functions() {
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