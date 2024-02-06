#include "notify.h"
#include "event_loop.h"
#include "io_event.h"
#include "log.h"
#include <cassert>
#include <cstdint>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace EasyNet;

Notify::Notify(EventLoop *loop) : IOEvent(loop, 0) {
#ifdef HAVE_EVENTFD
    int event_fd = ::eventfd(0, EFD_NONBLOCK);
    this->SetFD(event_fd);
#else
    int fd[2];
    int r = pipe(fd);
    assert(!r);
    this->SetFD(fd[0]);
    m_notifier = fd[1];
#endif
    this->EnableRead();
}

Notify::~Notify() {
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

void Notify::ProcessReadEvent() {
#ifdef HAVE_EVENTFD
    eventfd_t tmp;
    eventfd_read(this->m_fd, &tmp);
#else
    char buf[128];
    ssize_t r;
    while ((r = ::read(this->GetFD(), buf, sizeof(buf))) != 0) {
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

void Notify::WakeUp() {
#ifdef HAVE_EVENTFD
    eventfd_write(this->GetFD(), 1);
#else
    char c = 0;
    while (::write(m_notifier, &c, 1) != 1 && errno == EINTR) {
    }
#endif
}