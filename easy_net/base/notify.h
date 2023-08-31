#ifndef __EASYNET_NOTIFY_H
#define __EASYNET_NOTIFY_H

#include "fd_event.h"
#include <features.h>

#if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 9)
#    define HAVE_EVENTFD 1
#    include <sys/eventfd.h>
#endif

class notify : public fd_event {
 public:
    notify(event_loop *loop);
    void wakeup();

 public:
    void handle_read() override;

 private:
#ifndef HAVE_EVENTFD
    int m_notifier; // 使用pipe时
#endif
};

#endif // !__EASYNET_NOTIFY_H
