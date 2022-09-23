#ifndef __SUBREACTOR_POOL_H
#define __SUBREACTOR_POOL_H

#include "event_loop.h"
#include "subreactor.h"

#include <thread>

class subreactor_pool {
public:
    subreactor_pool(int cnt);
    subreactor* get_sub_reactor();

private:
    std::vector<subreactor*> sub_reactors_;
    int curr_idx_;
    int sub_reactor_cnt_; //最少有一个子线程,用于处理监听acceptfd
};

#endif