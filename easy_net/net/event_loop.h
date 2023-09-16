#ifndef __EASYNET_EVENT_LOOP_H
#define __EASYNET_EVENT_LOOP_H

#include <functional>
#include <memory>
#include <sys/epoll.h>

#include "def.h"
#include "non_copyable.h"
#include "notify.h"
#include "poller.h"

class event_loop : public non_copyable {
    using event_cb_f = std::function<void(event_loop *loop, int fd, void *args)>;

 public:
    event_loop();
    ~event_loop() = default;

    using pending_func_t = std::function<void()>;

 public:
    // loop
    void loop();

    // operator for timer event
    int run_at(event_cb_f cb, void *args, uint64_t ts);
    int run_after(event_cb_f cb, void *args, int sec, int mills);
    int run_every(event_cb_f cb, void *args, int sec, int mills);

    // 提供将函数注入loop的接口
    void run_in_loop(const pending_func_t &cb);
    void queue_in_loop(const pending_func_t &cb);

    // 唤醒event_loop
    void wakeup() {
        m_notifyer.wakeup();
    }

    void quit();

    std::unique_ptr<poller> &get_poller() { return m_poller; }

 private:
    void _do_pending_functions();

 private:
    bool m_quit;
    bool m_looping;
    poller::active_events_t m_ready_events; // 当前loop上就绪的fd
    std::unique_ptr<poller> m_poller;       // 管理IO多路复用
    bool m_pending_func;                    // 是否正在执行
    std::vector<pending_func_t> m_pending_func_list;
    notify m_notifyer; // 使用eventfd或者pipe来唤醒event_loop
};

#endif