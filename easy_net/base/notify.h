#ifndef __EASYNET_NOTIFY_H
#define __EASYNET_NOTIFY_H

#include "fd_event.h"
#include <features.h>

#if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 9)
#    define HAVE_EVENTFD 1
#endif

class notify : public fd_event {
 public:
    notify(event_loop *loop);
    void wakeup();

 private:
    int m_fd2; // 使用pipe时
};

#endif // !__EASYNET_NOTIFY_H
