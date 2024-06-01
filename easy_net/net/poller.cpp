#include "poller.h"

#include "epoll_poller.h"

using namespace EasyNet;

Poller::Poller(EventLoop *loop) : m_owner_loop(loop) {
    // nothing todo
}

Poller *Poller::CreatePoller(EventLoop *loop) {
    Poller *p = nullptr;

#ifdef __linux__
    p = new EpollPoller(loop);
#elif defined(__APPLE__)
    // TODO: kqueue
#elif defined(_WIN32)
    // TODO: iocp
#endif
    return p;
}