#include "tcp_connection.h"
#include "buffer.h"
#include "print_debug.h"
#include <cstdio>
#include <fcntl.h>

void tcp_connection::init(event_loop *loop, int fd)
{
    // 1,设置非阻塞
    acceptfd_ = fd;
    int flag  = ::fcntl(acceptfd_, F_GETFL, 0);
    ::fcntl(acceptfd_, F_SETFL, O_NONBLOCK | flag);

    loop_ = loop;
    loop_->add_io_event(
        acceptfd_, [&](event_loop *loop, int fd, void *args) { this->_handle_read(); }, EPOLLIN, this); // TCP连接已经建立好了,双方进入ESTABLISH状态，可以进行数据传输。
}

int tcp_connection::send_data(const char *data, size_t data_size)
{
    // todo：将数据填充到buffer中

    loop_->add_io_event(
        acceptfd_, [&](event_loop *loop, int fd, void *args) { this->_handle_write(); }, EPOLLOUT, this);
    return 0;
}

void tcp_connection::_handle_read()
{
    // 1,从fd中读取完所有数据并写入read_buf_中
    int err;
    auto read_size = read_fd_to_buf(*read_buf_, acceptfd_, err);
    // 2,触发回调,将数据返回给用户. 需要注意的是,每次调用user_cb后，read_buf_要清空.
    printf("read_size=%ld", read_size);
}

void tcp_connection::_handle_write()
{
    // todo
}