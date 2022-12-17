#include "event_loop.h"
#include "cb.h"
#include "event.h"
#include "print_debug.h"
#include <cstdio>
#include <sys/epoll.h>

event_loop::event_loop()
{
    // 初始化epoll
    this->epoll_fd_ = ::epoll_create1(0);
    if (this->epoll_fd_ == -1) {
        perror("epoll_create failed!");
        exit(-1);
    }
    // 初始化timer
}

void event_loop::process_event()
{
    for (;;) {
        int fd_cnts = ::epoll_wait(this->epoll_fd_, this->ready_events_, k_init_eventlist_size, k_timeouts_ms);
        if (fd_cnts > 0) {
            for (int i = 0; i < fd_cnts; i++) {
                auto it = io_events_.find(ready_events_[i].data.fd);
                if (it == io_events_.end()) {
                    break;
                }
                io_event_t *ev = &it->second;
                if (ready_events_[i].events & EPOLLIN) {
                    void *args = ev->r_cb_args;
                    ev->read_cb(this, ready_events_[i].data.fd, args);
                }
                if (ready_events_[i].events & EPOLLOUT) {
                    void *args = ev->w_cb_args;
                    ev->write_cb(this, ready_events_[i].data.fd, args);
                }
                if (ready_events_[i].events & (EPOLLHUP | EPOLLERR)) {
                    if (ev->read_cb) {
                        void *args = ev->r_cb_args;
                        ev->read_cb(this, ready_events_[i].data.fd, args);
                    } else if (ev->write_cb) {
                        void *args = ev->w_cb_args;
                        ev->write_cb(this, ready_events_[i].data.fd, args);
                    } else {
                        del_io_event(ready_events_[i].data.fd);
                    }
                }
            }
        } else if (fd_cnts == 0) {
            // nothing happen
        } else {
            // error happens
            // todo: log
        }
    }
}

void event_loop::add_io_event(int fd, event_cb_f cb, int mask, void *args)
{
    int final_flag = 0;
    int final_opt  = 0;
    auto it        = this->io_events_.find(fd);
    if (it != this->io_events_.end()) {
        final_flag = mask;
        final_opt  = EPOLL_CTL_MOD;
    } else {
        final_flag = mask;
        final_opt  = EPOLL_CTL_ADD;
    }
    this->io_events_[fd].flag = final_flag;

    if (mask & EPOLLIN) {
        this->io_events_[fd].read_cb   = cb;
        this->io_events_[fd].r_cb_args = args;
    }
    if (mask & EPOLLOUT) {
        this->io_events_[fd].write_cb  = cb;
        this->io_events_[fd].w_cb_args = args;
    }
    struct epoll_event ev;
    ev.events  = final_flag;
    ev.data.fd = fd;
    int ret    = ::epoll_ctl(this->epoll_fd_, final_opt, fd, &ev);
    if (ret == -1) {
        printfd("epoll_ctl error!");
        // log
    }
}

void event_loop::del_io_event(int fd)
{
    for (auto &n : io_events_) {
        printfd("fd:%d", n.first);
    }
    printfd("del fd=%d", fd);
    io_events_.erase(fd);
    ::epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
}

int event_loop::run_at(event_cb_f cb, void *args, uint64_t ts) {}