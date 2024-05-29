#ifndef __EASYNET_EPOLL_POLLER_H
#define __EASYNET_EPOLL_POLLER_H

#include "poller.h"

namespace EasyNet {
class EpollPoller : public Poller {
 public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller();

 public:
    void AddEvent(IOEvent *ev) override;
    void DelEvent(IOEvent *ev) override;
    void ModEvent(IOEvent *ev) override;
    void Polling(int timeout_ms, active_events_t &events) override;

 public:
    // 获取具体的事件
    std::string GetEventString(uint32_t ev) const;

 private:
    int m_epollfd;
};
}  // namespace EasyNet

#endif