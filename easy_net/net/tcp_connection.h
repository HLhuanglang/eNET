/*
** 每一个tcp连接都有一个tcp_connection对象,该对象
*/
#ifndef __TCP_CONNECTION_H
#define __TCP_CONNECTION_H

#include "buffer.h"
#include "event_loop.h"
#include <cstddef>
#include <vector>

class event_loop;
class tcp_connection {
public:
    tcp_connection(event_loop* loop, int fd)
    {
        init(loop, fd);
        read_buf_  = new read_buffer();
        write_buf_ = new write_buffer();
    }

public:
    void init(event_loop* loop, int fd);
    int send_data(const char* data, size_t data_size);

private:
    void _handle_read();
    void _handle_write();

private:
    event_loop* loop_;
    int acceptfd_;
    read_buffer* read_buf_;
    write_buffer* write_buf_;
};

#endif