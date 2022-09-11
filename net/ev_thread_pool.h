#ifndef __EV_THREAD_POOL_H
#define __EV_THREAD_POOL_H

#include "ev_thread.h"
#include "event_loop.h"

#include <thread>

class ev_thread_pool {
public:
    ev_thread_pool(int cnt);
    ev_thread* get_sub_reactor();

private:
    std::vector<ev_thread*> sub_reactor_;
    int curr_idx_;
    int sub_reactor_cnt_; //最少有一个子线程,用于处理监听acceptfd
};

#endif