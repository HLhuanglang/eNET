#ifndef __EASYNET_POLL_POLLER_H
#define __EASYNET_POLL_POLLER_H

#include "poller.h"

class poll_poller : public poller {
 public:
    poll_poller(event_loop *loop);
    ~poll_poller() override = default;

 public:
    void add_fd_event(fd_event *ev) override;
    void del_fd_event(fd_event *ev) override;
    void mod_fd_event(fd_event *ev) override;

    void polling(int timeout_ms, active_events_t &events) override;
};

#endif