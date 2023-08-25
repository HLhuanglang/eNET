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
#include "fd_event.h"

#include "buffer.h"
#include "cb.h"

class event_loop;
class tcp_connection : public std::enable_shared_from_this<tcp_connection>, public fd_event {
 public:
    tcp_connection(connection_owner *owner, int fd) : fd_event(owner->get_loop(), fd), m_owner(owner) {
        m_read_buf = new buffer();
        m_write_buf = new buffer();
    }

    ~tcp_connection() override {
        // 避免释放连接时候内存泄漏
        delete m_read_buf;
        delete m_write_buf;
    }

 public:
    // void init(event_loop* loop, int fd);
    void send_data(const char *data, size_t data_size);

    buffer &get_readbuf() { return *m_read_buf; }
    buffer &get_writebuf() { return *m_write_buf; }

 public:
    void handle_write() override;
    void handle_read() override;
    void handle_close() override;
    void handle_error() override;

 private:
    connection_owner *m_owner; // tcpclient或tcpserver会传入this指针，此时owner_指向tcpclient或tcpserver
    buffer *m_read_buf;
    buffer *m_write_buf;
};

using sp_tcp_connection_t = std::shared_ptr<tcp_connection>;
using new_connection_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using del_connection_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using recv_msg_cb_f = std::function<void(const sp_tcp_connection_t &, timestemp t)>;
using write_complete_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using high_water_mark_cb_f = std::function<void(const sp_tcp_connection_t &)>;

#endif