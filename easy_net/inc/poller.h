#ifndef __EASYNET_POLLER_H
#define __EASYNET_POLLER_H

#include <map>
#include <vector>

#include "non_copyable.h"

#include "cb.h"

class event_loop;
class fd_event;

enum class poller_type_e {
    TYPE_EPOLL,
    TYPE_POLL
};

class poller : public non_copyable {
public:
    using active_events_t = std::vector<fd_event *>;
    poller(event_loop *loop);

public:
    virtual ~poller() = 0;
    virtual void add_fd_event(fd_event *ev) = 0;
    virtual void del_fd_event(fd_event *ev) = 0;
    virtual void mod_fd_event(fd_event *ev) = 0;
    virtual void polling(int timeout_ms, active_events_t &events) = 0;

protected:
    std::map<int, fd_event *> fdmp_; //继承可用

private:
    event_loop *owner_loop_; //poller所属的event_loop
};

extern "C" poller *create_poller(poller_type_e type, event_loop *loop);

#endif