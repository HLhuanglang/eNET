#include "event_loop.h"
#include <cstdio>
#include <sys/epoll.h>

#include "fd_event.h"
#include "print_debug.h"

#include "cb.h"
#include "event.h"

event_loop::event_loop() : poller_(create_poller(poller_type_e::TYPE_EPOLL, this)) {
}

void event_loop::loop() {
    poller_->polling(10000, ready_events);
    quit_ = false;
    while (false == quit_) {
        //1,处理当前处于活动状态的fd上的事件
        for (auto &it : ready_events) {
            it->handle_event();
        }
        //2,处理外部线程注入的回调函数队列(eg：定时任务)
    }
}