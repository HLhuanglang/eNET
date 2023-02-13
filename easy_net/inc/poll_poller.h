#ifndef __EASYNET_POLL_POLLER_H
#define __EASYNET_POLL_POLLER_H

#include "cb.h"
#include "poller.h"

class poll_poller : public poller {
public:
    poll_poller(event_loop *loop);

public:
    virtual void add_fd_event(fd_event *ev) override;
    virtual void del_fd_event(fd_event *ev) override;
    virtual void mod_fd_event(fd_event *ev) override;

    virtual void polling() override;
};

#endif