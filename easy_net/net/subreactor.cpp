#include "subreactor.h"
#include "buffer.h"
#include "cb.h"
#include <cstdio>
#include <mutex>
#include <type_traits>
#include <unistd.h>
#include <utility>

#include <sys/eventfd.h>

#include "tcp_connection.h"
#include <fcntl.h>
#include "print_debug.h"

subreactor::subreactor()
{
    this->eventfd_ = ::eventfd(0, EFD_NONBLOCK);
    if (this->eventfd_ == -1) {
        perror("eventfd(0, EFD_NONBLOCK)");
        exit(-1);
    }
    printfd("eventfd=%d", this->eventfd_);
    thread_id_ = std::this_thread::get_id();
}

void subreactor::set_loop(std::shared_ptr<event_loop> loop, event_cb_f func, void* args)
{
    this->sp_loop_ = std::move(loop);
    this->sp_loop_.get()->add_io_event(this->eventfd_, std::move(func), EPOLLIN, args);
}

void subreactor::enable_accept(int fd)
{
    int flag = ::fcntl(fd, F_GETFL, 0);
    ::fcntl(fd, F_SETFL, O_NONBLOCK | flag);
    sp_loop_->add_io_event(
        fd, [&](event_loop* loop, int fd, void* args) { _handle_read(fd); }, EPOLLIN, this); // TCP连接已经建立好了,双方进入ESTABLISH状态，可以进行数据传输。
}

// 主要是main reactor调用
void subreactor::notify(const msg_t& msg)
{
    std::lock_guard<std::mutex> lg(this->mtx_);
    eventfd_msg_t t = 1;
    int ret         = ::write(this->eventfd_, &t, sizeof(eventfd_msg_t));
    if (ret == -1) {
        perror("write error!");
    }
    msgs_.emplace_back(msg);
}

// 子线程调用时会x主动阻塞住x,等main reactor发送了消息read才会返回,然后进行后续处理
// eventfd设置成了非阻塞模式，子线程主动调用wakeup并不会阻塞，read会直接返回。
void subreactor::wakeup(std::vector<msg_t>& msgs)
{
    std::lock_guard<std::mutex> lg(this->mtx_);
    eventfd_msg_t t;
    int ret = ::read(this->eventfd_, &t, sizeof(eventfd_msg_t));
    if (ret == -1) {
        perror("read error!");
    }
    std::swap(this->msgs_, msgs);
}

void subreactor::_handle_read(int fd)
{
    tcp_connection* conn = connection_map_[fd];
    //从这个地方回调到用户注册的接收消息回调
    if (conn != nullptr) {
        auto ret = conn->_handle_read();
        if (ret) {
            printfd("buf:\n%s", conn->get_readbuf().readable_start());
            msg_cb(*conn, conn->get_readbuf());
        } else {
            connection_map_.erase(fd);
            conn->_handle_close();
        }
    } else {
        //找不到链接???
    }
}