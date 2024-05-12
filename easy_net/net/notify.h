#ifndef __EASYNET_NOTIFY_H
#define __EASYNET_NOTIFY_H

#include <features.h>

#include "io_event.h"

#if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 9)
#    define HAVE_EVENTFD 1
#    include <sys/eventfd.h>
#endif

namespace EasyNet {
class Notify : public IOEvent {
 public:
    Notify(EventLoop *loop);
    ~Notify() override;

    void WakeUp();

 public:
    void ProcessReadEvent() override;

 private:
#ifndef HAVE_EVENTFD
    int m_notifier;  // 使用pipe时
#endif
};
}  // namespace EasyNet

#endif  // !__EASYNET_NOTIFY_H
