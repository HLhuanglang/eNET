#include "subreactor_pool.h"
#include "print_debug.h"
#include "tcp_connection.h"
#include <iostream>

void get_msg_cb(event_loop* loop, int fd, void* args)
{
    subreactor* mq = (subreactor*)args;
    std::vector<msg_t> msgs;
    mq->wakeup(msgs);
    for (auto& n : msgs) {
        if (n.msg_type_ == msg_type_t::NEW_CONN) {
            //创建新的连接
            tcp_connection* new_conn = new tcp_connection(loop, fd);
            //然后将acceptfd添加到epoll中进行监视.
            new_conn->init(loop, n.accept_fd_);
        }
        if (n.msg_type_ == msg_type_t::NEW_TASK) {
            //添加任务
            n.task_->func_(loop, n.task_->args_);
        }
    }
}

void thread_domain(subreactor* mq)
{
    std::shared_ptr<event_loop> sp_loop(new event_loop());
    mq->set_loop(sp_loop, get_msg_cb, mq);
    sp_loop.get()->process_event();
}

subreactor_pool::subreactor_pool(int cnt)
    : curr_idx_(0)
{
    if (cnt < k_sub_reactor_cnt) {
        sub_reactor_cnt_ = k_sub_reactor_cnt;
    } else {
        sub_reactor_cnt_ = cnt;
    }
    sub_reactors_.resize(sub_reactor_cnt_);
    for (int i = 0; i < sub_reactor_cnt_; i++) {
        subreactor* mq   = new subreactor();
        sub_reactors_[i] = mq;
        std::thread t(thread_domain, mq);
        t.detach();
    }
}

subreactor* subreactor_pool::get_sub_reactor()
{
    if (curr_idx_ == sub_reactor_cnt_) {
        curr_idx_ = 0;
    }
    return sub_reactors_[curr_idx_++];
}