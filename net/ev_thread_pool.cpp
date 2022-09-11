#include "ev_thread_pool.h"
#include <iostream>

void get_msg_cb(event_loop* loop, int fd, void* args) {
    ev_thread* mq = (ev_thread*)args;
    std::vector<msg_t> msgs;
    mq->wakeup(msgs);
    for (auto& n : msgs) {
        if (n.msg_type_ == msg_type_t::NEW_CONN) {
            //创建新的连接
        }
        if (n.msg_type_ == msg_type_t::NEW_TASK) {
            //添加任务
            std::cout << "thread:" << mq->get_threadid() << "  ";
            n.task_->func_(loop, n.task_->args_);
        }
    }
}

void thread_domain(ev_thread* mq) {
    std::shared_ptr<event_loop> sp_loop(new event_loop());
    mq->set_loop(sp_loop, get_msg_cb, mq);
    sp_loop.get()->process_event();
}

ev_thread_pool::ev_thread_pool(int cnt) : curr_idx_(0) {
    if (cnt < k_sub_reactor_cnt) {
        sub_reactor_cnt_ = k_sub_reactor_cnt;
    }
    else {
        k_sub_reactor_cnt = cnt;
    }
    sub_reactor_.resize(sub_reactor_cnt_);
    for (int i = 0; i < sub_reactor_cnt_; i++) {
        ev_thread* mq = new ev_thread();
        sub_reactor_[i] = mq;
        std::thread t(thread_domain, mq);
        t.detach();
    }
}

ev_thread* ev_thread_pool::get_sub_reactor() {
    if (curr_idx_ == sub_reactor_cnt_) {
        curr_idx_ = 0;
    }
    return sub_reactor_[curr_idx_++];
}