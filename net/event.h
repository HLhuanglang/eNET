/*
** 网络编程主要处理io事件和定时器事件
*/
#ifndef __EVENT_H
#define __EVENT_H

#include "cb.h"
#include <functional>
#include <stdint.h>

typedef struct io_event_t {
    event_cb_f read_cb;  // for EPOLLIN
    event_cb_f write_cb; // for EPOLLOUT
    void *r_cb_args;
    void *w_cb_args;
    int flag;
} io_event_t;

typedef struct timer_event_t {
    event_cb_f cb;
    void *cb_data;
    uint64_t ts; // timespec
    uint32_t interval;
    int timer_id;
} timer_event_t;
#endif