#include "notify.h"
#include <cassert>
#include <cstdint>
#include <unistd.h>

notifier::notifier() {
    int fd[2];
    int r = pipe(fd);
    assert(!r);
    m_notifier = fd[1];
    m_reciver = fd[0];
}

void notifier::wakeup() const {
    uint64_t msg = 1;
    ::write(m_notifier, &msg, sizeof(msg));
}

void notifier::handle_read() const {
    uint64_t msg = 1;
    ::read(m_reciver, &msg, sizeof(msg));
}