#include "poller.h"

#include "epoll_poller.h"
#include "poll_poller.h"

using namespace EasyNet;

Poller::Poller(EventLoop *loop) : m_owner_loop(loop) {
    // nothing todo
}

Poller *Poller::CreatePoller(poller_type_e type, EventLoop *loop) {
    Poller *p = nullptr;
    switch (type) {
        case poller_type_e::TYPE_EPOLL: {
            p = new EpollPoller(loop);
            break;
        }
        case poller_type_e::TYPE_POLL: {
            p = new PollPoller(loop);
            break;
        }
    }

    return p;
}