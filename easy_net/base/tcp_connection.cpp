#include "tcp_connection.h"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>

#include "fd_event.h"
#include "socket_opt.h"

#include "buffer.h"
#include "log.h"

void tcp_connection::send_data(const char *data, size_t data_size) {
    // 这个函数主要是为了做异步发送，客户端直接往buf里面写，不让客户端阻塞在write上面
    // 上层调用的,只要判断buf中有数据就将EPOLLOUT事件添加到epoll,传入回调。在回调中调用write_buf_to_fd
    //(上层调用有个小优化逻辑,就是数据必须达到多少量才发送,不然频繁的发送小数据不划算...当然如果只发一点点无法触发发送也是不行的，可以增加一个定时器操作,如果达到多少时间后数据量还是不够,就直接发送)
    // 当buf写空了以后才把EPOLLOUT事件去除。

    // 1,先写入缓存中
    m_write_buf->append(data, data_size);

    // 2,判断当前数据是否真的需要发送
    if (m_write_buf->readable_size() > 0) { // fixme：定时器+数据量，双重判断是否需要发送
        this->enable_writing(epoll_opt_e::MOD_EVENT);
    }
}

void tcp_connection::handle_read() {
    int err;
    auto n = socket_opt::read_fd_to_buf(*m_read_buf, this->get_fd(), err);
    if (n > 0) {
        m_owner->recv_msg(shared_from_this());
    } else if (n == 0) {
        // 对端关闭了连接
        this->handle_close();
    } else {
        if (err == EAGAIN || err == EWOULDBLOCK) {
            // 对端没有数据可读
            return;
        }
        this->handle_error();
    }
}

void tcp_connection::handle_write() {
    while (m_write_buf->readable_size() != 0U) {
        auto ret = socket_opt::write_buf_to_fd(*m_write_buf, this->get_fd());
        if (ret < 0) {
            this->handle_error();
        }
        if (ret == 0) {
            // 当前写缓冲区已经满了,再次尝试写入
            break;
        }
    }
    if (m_write_buf->readable_size() == 0) {
        // 无数据可写了,将EPOLLOUT事件删除，避免一直触发
        this->disable_writing(epoll_opt_e::MOD_EVENT);
        m_owner->write_complete(shared_from_this());
    }
}

void tcp_connection::handle_close() {
    m_owner->del_connection(shared_from_this());
}

void tcp_connection::handle_error() {
    LOG_FATAL("handle fd=%d error, %s", this->get_fd(), strerror(errno));
}