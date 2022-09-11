#include "ev_thread.h"
#include <cstdio>
#include <mutex>
#include <type_traits>
#include <unistd.h>

#include <sys/eventfd.h>

ev_thread::ev_thread() {
    this->eventfd_ = ::eventfd(0, EFD_NONBLOCK);
    if (this->eventfd_ == -1) {
        perror("eventfd(0, EFD_NONBLOCK)");
        exit(-1);
    }
}

void ev_thread::set_loop(std::shared_ptr<event_loop> loop, event_cb_f func, void* args) {
    this->sp_loop_ = loop;
    this->sp_loop_.get()->add_io_event(this->eventfd_, func, EPOLLIN, args);
}

// main reactor可能一次性收到很多连接请求,accept完事后,然后通过调度算法分配给子reactor
// 去做后续数据通信的监听
// 可能的问题在于如果连接数量很大,vector会暴涨,好在元素是pod类型和指针,直接释放就行,不会导致内存泄露.
//
// 主要是main reactor调用
void ev_thread::notify(const msg_t& msg) {
    std::lock_guard<std::mutex> lg(this->mtx_);
    eventfd_msg_t t = 1;
    int ret = ::write(this->eventfd_, &t, sizeof(eventfd_msg_t));
    if (ret == -1) {
        perror("write error!");
    }
    msgs_.emplace_back(msg);
}

// 子线程调用时会主动阻塞住,等main reactor发送了消息read才会返回,然后进行后续处理
void ev_thread::wakeup(std::vector<msg_t>& msgs) {
    std::lock_guard<std::mutex> lg(this->mtx_);
    eventfd_msg_t t;
    int ret = ::read(this->eventfd_, &t, sizeof(eventfd_msg_t));
    if (ret == -1) {
        perror("read error!");
    }
    std::swap(this->msgs_, msgs);
}