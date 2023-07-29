#include "epoll_poller.h"
#include <array>
#include <strings.h>
#include <sys/epoll.h>

#include "fd_event.h"

#include "cb.h"
#include "log.h"

// epoll、poll使用一样的值
static_assert(EPOLLIN == POLLIN, "EPOLLIN");
static_assert(EPOLLPRI == POLLPRI, "EPOLLPRI");
static_assert(EPOLLOUT == POLLOUT, "EPOLLOUT");
static_assert(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP");
static_assert(EPOLLERR == POLLERR, "EPOLLERR");
static_assert(EPOLLHUP == POLLHUP, "EPOLLHUPEPOLLHUP");

epoll_poller::epoll_poller(event_loop *loop) : poller(loop) {
    m_epollfd = ::epoll_create1(0);
    if (m_epollfd < 0) {
        LOG_FATAL("epoll_create1 fail");
    }
}

// typedef union epoll_data
// {
//     void *ptr;
//     int fd;
//     uint32_t u32;
//     uint64_t u64;
// }
// epoll_data_t;

// struct epoll_event {
//     uint32_t events;   /* Epoll events */
//     epoll_data_t data; /* User data variable */
// }

void epoll_poller::polling(int timeout_ms, active_events_t &events) {
    struct epoll_event epoll_events[1024];
    int nfds = ::epoll_wait(m_epollfd, epoll_events, 1024, timeout_ms);
    if (nfds < 0) {
        LOG_FATAL("epoll_wait fail");
    }

    for (int i = 0; i < nfds; ++i) {
        auto *ev = static_cast<fd_event *>(epoll_events[i].data.ptr);
        ev->set_revents(epoll_events[i].events);
        events.push_back(ev);
    }
}

void epoll_poller::add_fd_event(fd_event *ev) {
    int fd = ev->get_fd();
    int events = ev->get_events();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) < 0) {
        LOG_FATAL("add fd = %d events = %d fail", fd, events);
    } else {
        LOG_DEBUG("add %d", fd);
    }
}

void epoll_poller::del_fd_event(fd_event *ev) {
    int fd = ev->get_fd();
    int events = ev->get_events();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &event) < 0) {
        LOG_FATAL("del fd = %d events = %d fail", fd, events);
    } else {
        m_fdmp[fd] = ev;
        LOG_DEBUG("del %d", fd);
    }
}

void epoll_poller::mod_fd_event(fd_event *ev) {
    int fd = ev->get_fd();
    int events = ev->get_events();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &event) < 0) {
        LOG_FATAL("mod fd = %d events = %d fail", fd, events);
    } else {
        m_fdmp[fd] = ev;
        LOG_DEBUG("mod %d", fd);
    }
}