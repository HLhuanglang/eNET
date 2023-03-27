#ifndef __EASYNET_EVENT_LOOP_H
#define __EASYNET_EVENT_LOOP_H

#include <functional>
#include <memory>
#include <sys/epoll.h>

#include "non_copyable.h"

#include "cb.h"
#include "def.h"
#include "event.h"
#include "poller.h"

class event_loop : public non_copyable {
public:
    event_loop();
    virtual ~event_loop();

    using pending_func_t = std::function<void()>;

public:
    // loop
    void loop();

    // operator for timer event
    int run_at(event_cb_f cb, void *args, uint64_t ts);
    int run_after(event_cb_f cb, void *args, int sec, int mills);
    int run_every(event_cb_f cb, void *args, int sec, int mills);

    // 提供将函数注入loop的接口
    void run_in_loop(const pending_func_t &cb);
    void queue_in_loop(const pending_func_t &cb);

    std::unique_ptr<poller> &get_poller() { return poller_; }

private:
    bool quit_;
    bool looping_;
    bool pending_func_;
    poller::active_events_t ready_events; //当前loop上就绪的fd
    std::unique_ptr<poller> poller_;      //管理IO多路复用
};

#endif