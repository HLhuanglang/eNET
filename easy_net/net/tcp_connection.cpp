#include "tcp_connection.h"
#include "buffer.h"
#include "cb.h"
#include "print_debug.h"
#include <cstddef>
#include <cstdio>
#include <fcntl.h>
#include "socket_opt.h"

// void tcp_connection::init(event_loop *loop, int fd)
// {
//     // 1,设置非阻塞
//     acceptfd_ = fd;
//     int flag  = ::fcntl(acceptfd_, F_GETFL, 0);
//     ::fcntl(acceptfd_, F_SETFL, O_NONBLOCK | flag);

//     loop_ = loop;
//     loop_->add_io_event(
//         acceptfd_, [&](event_loop *loop, int fd, void *args) { this->_handle_read(); }, EPOLLIN, this); // TCP连接已经建立好了,双方进入ESTABLISH状态，可以进行数据传输。
// }

void tcp_connection::send_data(const char *data, size_t data_size)
{
    bool need_send = false;
    if (write_buf_->readable_size() > 0) {
        need_send = true;
    }

    //将要发送的数据写入buf中.
    //
    //这个函数主要是为了做异步发送，客户端直接往buf里面写，不让客户端阻塞在write上面
    //上层调用的,只要判断buf中有数据就将EPOLLOUT事件添加到epoll,传入回调。在回调中调用write_buf_to_fd
    //(上层调用有个小优化逻辑,就是数据必须达到多少量才发送,不然频繁的发送小数据不划算...当然如果只发一点点无法触发发送也是不行的，可以增加一个定时器操作,如果达到多少时间后数据量还是不够,就直接发送)
    //当buf写空了以后才把EPOLLOUT事件去除。
    //write_buf_->append(data, data_size);

    if (need_send) {
        loop_->add_io_event(
            acceptfd_, [&](event_loop *loop, int fd, void *args) { this->_handle_write(); }, EPOLLOUT, this);
    }
}

size_t tcp_connection::read_data()
{
    int err;
    return socket_opt::read_fd_to_buf(*read_buf_, acceptfd_, err);
}

void tcp_connection::_handle_write()
{
    while (write_buf_->readable_size()) {
        //printfd("send buf:\n%s", write_buf_->readable_start());
        auto ret = socket_opt::write_buf_to_fd(*write_buf_, acceptfd_);
        if (ret < 0) {
            //写acceptfd出错了,直接断开连接...
            _handle_close();
        }
        if (ret == 0) {
            //当前写缓冲区已经满了,再次尝试写入
            break;
        }
    }
    if (write_buf_->readable_size() == 0) {
        //无数据可写了,将EPOLLOUT事件删除，避免一直触发
        _disable_write();
    }
}

void tcp_connection::_handle_close()
{
    loop_->del_io_event(acceptfd_);
}

void tcp_connection::_disable_write()
{
    //fixme：从监听EPOLLOUT|EPOLLIN 变成只监听EPOLLIN ????
    loop_->update_io_event(acceptfd_, EPOLLIN);
}