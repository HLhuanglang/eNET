/*
    tcp通信的本质就是建立一条tcp链接，在这条链接中发生读写事件，进而触发读写回调执行相应的逻辑
*/
#ifndef __EASYNET_TCP_CONNECTION_H
#define __EASYNET_TCP_CONNECTION_H

#include <cstddef>
#include <memory>
#include <vector>

#include "connection_owner.h"
#include "event_loop.h"

#include "buffer.h"
#include "cb.h"

class event_loop;
class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    tcp_connection(event_loop *loop, int fd) {
        //init(loop, fd);
        loop_ = loop;
        acceptfd_ = fd;
        read_buf_ = new buffer();
        write_buf_ = new buffer();
    }

    ~tcp_connection() {
        //避免释放连接时候内存泄漏
        if (read_buf_) {
            delete read_buf_;
        }
        if (write_buf_) {
            delete write_buf_;
        }
    }

public:
    //void init(event_loop* loop, int fd);
    void send_data(const char *data, size_t data_size);
    size_t read_data();

    buffer &get_readbuf() { return *read_buf_; }
    buffer &get_writebuf() { return *write_buf_; }

public:
    void _handle_write();
    void _handle_close();
    void _disable_write();

private:
    event_loop *loop_;
    int acceptfd_;
    buffer *read_buf_;
    buffer *write_buf_;

    connection_owner *owner_; //当前这条链接属于客户端还是服务端
};

#endif