/*
** 每一个tcp连接都有一个tcp_connection对象,该对象
*/
#ifndef __TCP_CONNECTION_H
#define __TCP_CONNECTION_H

#include "buffer.h"
#include "cb.h"
#include "event_loop.h"
#include <cstddef>
#include <vector>

class event_loop;
class tcp_connection {
public:
    tcp_connection(event_loop* loop, int fd)
    {
        //init(loop, fd);
        loop_      = loop;
        acceptfd_  = fd;
        read_buf_  = new buffer();
        write_buf_ = new buffer();
    }

public:
    //void init(event_loop* loop, int fd);
    int send_data(const char* data, size_t data_size);

    buffer& get_readbuf() { return *read_buf_; }
    buffer& get_writebuf() { return *write_buf_; }

public:
    size_t _handle_read();
    size_t _handle_write();
    void _handle_close();

private:
    event_loop* loop_;
    int acceptfd_;
    buffer* read_buf_;
    buffer* write_buf_;
};

#endif