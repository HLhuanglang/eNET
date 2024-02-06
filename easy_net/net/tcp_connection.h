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
#include "io_event.h"

#include "buffer.h"

namespace EasyNet {

// 前置声明
class EventLoop;

class TcpConn : public std::enable_shared_from_this<TcpConn>, public IOEvent {

 public:
    TcpConn(ConnOwner *owner, int fd) : IOEvent(owner->GetEventLoop(), fd), m_owner(owner) {
        m_read_buf = new Buffer();
        m_write_buf = new Buffer();
    }

    ~TcpConn() override {
        // 避免释放连接时候内存泄漏
        delete m_read_buf;
        delete m_write_buf;
    }

 public:
    void SendData(const char *data, size_t data_size);
    Buffer &GetReadBuf() { return *m_read_buf; }
    Buffer &GetWriteBuf() { return *m_write_buf; }

 public:
    void ProcessWriteEvent() override;
    void ProcessReadEvent() override;

 private:
    ConnOwner *m_owner; // tcpclient或tcpserver会传入this指针，此时owner_指向tcpclient或tcpserver
    Buffer *m_read_buf;
    Buffer *m_write_buf;
};

using sp_tcp_connection_t = std::shared_ptr<TcpConn>;
using new_connection_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using del_connection_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using recv_msg_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using write_complete_cb_f = std::function<void(const sp_tcp_connection_t &)>;
using high_water_mark_cb_f = std::function<void(const sp_tcp_connection_t &, size_t mark)>;
} // namespace EasyNet

#endif