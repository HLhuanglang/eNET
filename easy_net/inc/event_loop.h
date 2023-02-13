#ifndef __EASYNET_EVENT_LOOP_H
#define __EASYNET_EVENT_LOOP_H

#include <memory>
#include <sys/epoll.h>

#include <unordered_map>

#include "cb.h"
#include "def.h"
#include "event.h"
#include "poller.h"

class event_loop {
public:
    event_loop();

public:
    // loop
    void process_event();

    // operator for io event
    void add_io_event(int fd, event_cb_f cb, int mask, void *args);
    void del_io_event(int fd);
    void update_io_event(int fd, int mask);

    // operator for timer event
    int run_at(event_cb_f cb, void *args, uint64_t ts);
    int run_after(event_cb_f cb, void *args, int sec, int mills);
    int run_every(event_cb_f cb, void *args, int sec, int mills);

    std::unique_ptr<poller> &get_poller() { return poller_; }

private:
    int epoll_fd_;
    struct epoll_event ready_events_[k_init_eventlist_size]; //储存epoll_wait返回的fd
    std::unordered_map<int, io_event_t> io_events_;          //方便ready_events_中所有的fd找到自己的事件处理函数
    std::unique_ptr<poller> poller_;                         //管理IO多路复用
};

#endif