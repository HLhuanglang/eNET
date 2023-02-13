/*
    管理所有fd上所有的事件：读写事件
*/
#ifndef __EASYNET_FD_EVENT_H
#define __EASYNET_FD_EVENT_H

#include "non_copyable.h"

#include "cb.h"

#ifdef __linux__
#    include <sys/poll.h>
#endif

const int k_non_event = 0;
const int k_read_event = POLLIN | POLLPRI;
const int k_write_evnet = POLLOUT;

enum epoll_opt_e {
    ADD_EVENT,
    DEL_EVENT,
    MOD_EVENT
};

class fd_event : public non_copyable {
public:
    fd_event(event_loop *loop, int fd)
        : ioloop_(loop),
          fd_(fd),
          events_(0),
          revents_(0),
          index_(-1) {}

    virtual ~fd_event() {}

    void enable_reading(epoll_opt_e optflag);
    void enable_writing(epoll_opt_e optflag);
    void disable_reading(epoll_opt_e optflag);
    void disable_writing(epoll_opt_e optflag);
    void disable_all(epoll_opt_e optflag);
    void update_event(epoll_opt_e optflag);

    // 虚函数，派生类可选择是否重写
    virtual void handle_event();
    virtual void handle_read();
    virtual void handle_write();
    virtual void handle_close();
    virtual void handle_error();

    // 查询、修改类属性
    bool is_none_event() const { return events_ == k_non_event; }
    bool is_writing() const { return events_ & k_write_evnet; }
    bool is_reading() const { return events_ & k_read_event; }

    //获取成员变量值
    int get_fd() { return fd_; }
    int get_events() { return events_; }
    int get_revents() const { return revents_; }
    void set_revents(int revt) { revents_ = revt; }
    int get_index() { return index_; }
    void set_index(int idx) { index_ = idx; }

private:
    event_loop *ioloop_; // 负责处理本描述符的IO线程
    int fd_;             // 被监控的文件描述符
    int events_;         // 用户设置期望监听的事件
    int revents_;        // Poller返回实际监听得到的事件
    int index_;          // 记录fd_event在vector中的位置used by Poll
};

#endif