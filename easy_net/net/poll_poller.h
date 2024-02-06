#ifndef __EASYNET_POLL_POLLER_H
#define __EASYNET_POLL_POLLER_H

#include "poller.h"

namespace EasyNet {
class PollPoller : public Poller {
 public:
    PollPoller(EventLoop *loop);
    ~PollPoller() override = default;

 public:
    void AddEvent(IOEvent *ev) override;
    void DelEvent(IOEvent *ev) override;
    void ModEvent(IOEvent *ev) override;

    void Polling(int timeout_ms, active_events_t &events) override;
};
} // namespace EasyNet

#endif