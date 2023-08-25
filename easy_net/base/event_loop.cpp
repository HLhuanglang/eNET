#include "event_loop.h"
#include <cstdio>
#include <sys/epoll.h>

#include "fd_event.h"

#include "cb.h"

event_loop::event_loop() : m_poller(create_poller(poller_type_e::TYPE_EPOLL, this)) {
}

void event_loop::loop() {
    m_poller->polling(10000, m_ready_events);
    m_quit = false;
    while (!m_quit) {
        // 1,处理当前处于活动状态的fd上的事件
        for (auto &it : m_ready_events) {
            it->handle_event();
        }
        // 2,处理外部线程注入的回调函数队列(eg：定时任务)
    }
}