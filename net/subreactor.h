#ifndef __SUBREACTOR_H
#define __SUBREACTOR_H

#include "cb.h"
#include "event_loop.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

using eventfd_msg_t = std::uint64_t;

enum class msg_type_t {
    NEW_CONN,
    NEW_TASK,
};
struct task_t {
    task_cb_f func_ = nullptr;
    void* args_ = nullptr;
};

struct msg_t {
    msg_type_t msg_type_;
    union {
        int accept_fd_;
        task_t* task_;
    };
};

class event_loop;
class subreactor {
public:
    subreactor();

public:
    void notify(const msg_t& msg);         //用于main reactor处理了连接事件后,把acceptfd传送给子进程并让其开始监听
    void wakeup(std::vector<msg_t>& msgs); //子进程接收main reactor发送的通知和数据
    std::shared_ptr<event_loop> get_loop() { return sp_loop_; }
    void set_loop(std::shared_ptr<event_loop> loop, event_cb_f func, void* args = nullptr);
    std::thread::id get_threadid() { return std::this_thread::get_id(); }

private:
    int eventfd_;                         // linux下用于事件通知的fd
    std::shared_ptr<event_loop> sp_loop_; // one thread peer loop
    std::vector<msg_t> msgs_;
    std::mutex mtx_;
};

#endif