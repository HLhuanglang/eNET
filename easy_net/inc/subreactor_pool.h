#ifndef __EASYNET_SUBREACTOR_POOL_H
#define __EASYNET_SUBREACTOR_POOL_H

#include <thread>

#include "event_loop.h"

#include "subreactor.h"

class subreactor_pool {
public:
    subreactor_pool(int cnt);
    subreactor *get_sub_reactor();
    subreactor *get_sub_reactor_by_id(int id);
    int get_pool_size() { return sub_reactor_cnt_; }
    int get_subreactor_id() { return curr_idx_; }

private:
    std::vector<subreactor *> sub_reactors_;
    int curr_idx_;
    int sub_reactor_cnt_; //最少有一个子线程,用于处理监听acceptfd
};

#endif