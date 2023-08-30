#include "poll_poller.h"

poll_poller::poll_poller(event_loop *loop) : poller(loop) {
    // todo
}

void poll_poller::add_fd_event(fd_event *ev) {
    // todo
}

void poll_poller::del_fd_event(fd_event *ev) {
    // todo
}

void poll_poller::mod_fd_event(fd_event *ev) {
    // todo
}

void poll_poller::polling(int timeout_ms, active_events_t &events) {
    // todo
}