#include "notify.h"
#include "event_loop.h"
#include "fd_event.h"
#include <cassert>
#include <cstdint>
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

void notify::handle_read() {
    uint64_t msg;
    ::read(this->get_fd(), &msg, sizeof(uint64_t));
}

void notify::wakeup() {
    uint64_t msg;
#ifdef HAVE_EVENTFD
    ::write(this->get_fd(), &msg, sizeof(uint64_t));
#else
    ::write(m_notifier, &msg, sizeof(uint64_t));
#endif
}