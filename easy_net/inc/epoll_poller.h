#ifndef __EASYNET_EPOLL_POLLER_H
#define __EASYNET_EPOLL_POLLER_H

#include "cb.h"
#include "poller.h"

class epoll_poller : public poller {
public:
    epoll_poller(event_loop *loop);

public:
    virtual void add_fd_event(fd_event *ev) override;
    virtual void del_fd_event(fd_event *ev) override;
    virtual void mod_fd_event(fd_event *ev) override;

    virtual void polling() override;

private:
    int epollfd_;
};

#endif