/*
** 网络编程主要处理io事件和定时器事件
*/
#ifndef __EASYNET_EVENT_H
#define __EASYNET_EVENT_H

#include <cstdint>
#include <functional>

#include "cb.h"

using io_event_t = struct io_event_t {
    event_cb_f read_cb;  // for EPOLLIN
    event_cb_f write_cb; // for EPOLLOUT
    event_cb_f close_cb; // for EPOLLHUB
    event_cb_f error_cb; // for EPOLLERR
    void *r_cb_args;
    void *w_cb_args;
    void *c_cb_args;
    void *e_cb_args;
    int flag;
};

using timer_event_t = struct timer_event_t {
    event_cb_f cb;
    void *cb_data;
    uint64_t ts; // timespec
    uint32_t interval;
    int timer_id;
};
#endif