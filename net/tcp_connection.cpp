#include "tcp_connection.h"
#include "print_debug.h"
#include <fcntl.h>

static void tcp_read_cb(event_loop *loop, int fd, void *args) {
    tcp_connection *conn = (tcp_connection *)(args);
    conn->handle_read();
}
static void tcp_write_cb(event_loop *loop, int fd, void *args) {
    tcp_connection *conn = (tcp_connection *)(args);
    conn->handle_write();
}

void tcp_connection::init(event_loop *loop, int fd) {
    // 1,设置非阻塞
    acceptfd_ = fd;
    int flag = ::fcntl(acceptfd_, F_GETFL, 0);
    ::fcntl(acceptfd_, F_SETFL, O_NONBLOCK | flag);

    loop_ = loop;
    loop_->add_io_event(acceptfd_, tcp_read_cb, EPOLLIN, this); // TCP连接已经建立好了,双方进入ESTABLISH状态，可以进行数据传输。
}

int tcp_connection::send_data(const char *data, size_t data_size) {
    // todo：将数据填充到buffer中

    loop_->add_io_event(acceptfd_, tcp_write_cb, EPOLLOUT, this);
    return 0;
}

void tcp_connection::handle_read() {
    // todo
    read_buf_->read(acceptfd_);
    printfd("buf=%s", read_buf_->data());
}

void tcp_connection::handle_write() {
    // todo
}