#ifndef __EASYNET_EPOLL_POLLER_H
#define __EASYNET_EPOLL_POLLER_H

#include "cb.h"
#include "poller.h"

class epoll_poller : public poller {
 public:
    epoll_poller(event_loop *loop);

 public:
    void add_fd_event(fd_event *ev) override;
    void del_fd_event(fd_event *ev) override;
    void mod_fd_event(fd_event *ev) override;

    void polling(int timeout_ms, active_events_t &events) override;

 private:
    int m_epollfd;
};

#endif