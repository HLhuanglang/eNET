#ifndef __EASYNET_POLLER_H
#define __EASYNET_POLLER_H

#include <map>
#include <vector>

#include "io_event.h"
#include "non_copyable.h"

namespace EasyNet {

class EventLoop;
class IOEvent;

enum class poller_type_e {
    TYPE_EPOLL,
    TYPE_POLL
};

class Poller : public NonCopyable {
 public:
    Poller(EventLoop *loop);

 public:
    virtual ~Poller() = default;
    virtual void AddEvent(IOEvent *ev) = 0;
    virtual void DelEvent(IOEvent *ev) = 0;
    virtual void ModEvent(IOEvent *ev) = 0;
    virtual void Polling(int timeout_ms, active_events_t &events) = 0;

 private:
    EventLoop *m_owner_loop; // poller所属的event_loop，不由poller控制生命周期
};

extern "C" Poller *create_poller(poller_type_e type, EventLoop *loop);
} // namespace EasyNet

#endif