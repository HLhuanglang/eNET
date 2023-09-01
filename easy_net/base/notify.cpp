#include "notify.h"
#include "event_loop.h"
#include "fd_event.h"
#include "log.h"
#include <cassert>
#include <cstdint>
#include <sys/eventfd.h>
#include <unistd.h>

notify::notify(event_loop *loop) : fd_event(loop, 0) {
#ifdef HAVE_EVENTFD
    int event_fd = ::eventfd(0, EFD_NONBLOCK);
    this->set_fd(event_fd);
#else
    int fd[2];
    int r = pipe(fd);
    assert(!r);
    this->set_fd(fd[0]);
    m_notifier = fd[1];
#endif
    this->enable_reading(epoll_opt_e::ADD_EVENT);
}

notify::~notify() {
    if (this->m_fd >= 0) {
        close(this->m_fd);
    }
    this->m_fd = -1;
#ifndef HAVE_EVENTFD
    if (this->m_notifier >= 0) {
        close(this->m_notifier);
    }
    this->m_notifier = -1;
#endif
}

void notify::handle_read() {
#ifdef HAVE_EVENTFD
    eventfd_t tmp;
    eventfd_read(this->m_fd, &tmp);
#else
    char buf[128];
    ssize_t r;
    while ((r = ::read(this->get_fd(), buf, sizeof(buf))) != 0) {
        if (r > 0) {
            continue;
        }
        switch (errno) {
            case EAGAIN:
                return;
            case EINTR:
                continue;
            default:
                return;
        }
    }
#endif
}

void notify::wakeup() {
#ifdef HAVE_EVENTFD
    eventfd_write(this->get_fd(), 1);
#else
    char c = 0;
    while (::write(m_notifier, &c, 1) != 1 && errno == EINTR) {
    }
#endif
}