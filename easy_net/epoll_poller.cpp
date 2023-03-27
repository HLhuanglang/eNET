#include "epoll_poller.h"
#include <strings.h>
#include <sys/epoll.h>

#include "fd_event.h"
#include "print_debug.h"

#include "cb.h"

//epoll、poll使用一样的值
static_assert(EPOLLIN == POLLIN, "EPOLLIN");
static_assert(EPOLLPRI == POLLPRI, "EPOLLPRI");
static_assert(EPOLLOUT == POLLOUT, "EPOLLOUT");
static_assert(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP");
static_assert(EPOLLERR == POLLERR, "EPOLLERR");
static_assert(EPOLLHUP == POLLHUP, "EPOLLHUPEPOLLHUP");

epoll_poller::epoll_poller(event_loop *loop) : poller(loop) {
    epollfd_ = ::epoll_create1(0);
    if (epollfd_ < 0) {
        //todo：LOG_FATAIL
    }
}

void epoll_poller::polling(int timeout_ms, active_events_t &events) {
    //todo
}

void epoll_poller::add_fd_event(fd_event *ev) {
    int fd = ev->get_fd();
    int events = ev->get_events();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        //LOG_FATAL("add fd = %d events = %d fail", inputFd, inputEvent);
    } else {
        //fdEventMap_[inputFd] = pFdEvent;
        //LOG_TRACE("add fd = %d; events = %d", inputFd, inputEvent);
        printfd("add %d", fd);
    }
}

void epoll_poller::del_fd_event(fd_event *ev) {
    int fd = ev->get_fd();
    int events = ev->get_events();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event) < 0) {
        //LOG_FATAL("add fd = %d events = %d fail", inputFd, inputEvent);
    } else {
        fdmp_[fd] = ev;
        //LOG_TRACE("add fd = %d; events = %d", inputFd, inputEvent);
        printfd("del %d", fd);
    }
}

void epoll_poller::mod_fd_event(fd_event *ev) {
    int fd = ev->get_fd();
    int events = ev->get_events();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
        //LOG_FATAL("add fd = %d events = %d fail", inputFd, inputEvent);
    } else {
        fdmp_[fd] = ev;
        //LOG_TRACE("add fd = %d; events = %d", inputFd, inputEvent);
        printfd("mod %d", fd);
    }
}