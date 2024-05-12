/*
    tcp通信的本质就是建立一条tcp链接，在这条链接中发生读写事件，进而触发读写回调执行相应的逻辑
*/
#ifndef __EASYNET_TCP_CONNECTION_H
#define __EASYNET_TCP_CONNECTION_H

#include <chrono>
#include <cstddef>
#include <memory>

#include "buffer.h"
#include "connection_owner.h"
#include "inet_addr.h"
#include "io_event.h"
#include "socket_opt.h"

namespace EasyNet {

// 前置声明
class EventLoop;

class TcpConn : public std::enable_shared_from_this<TcpConn>, public IOEvent {
 public:
    enum class ConnStatus {
        CONNECTING,     // 正在连接
        CONNECTED,      // 已经连接
        DISCONNECTING,  // 正在断开连接(给客户端使用的,就是只关闭写端)
        DISCONNECTED,   // 断开连接
    };

 public:
    TcpConn(ConnOwner *owner, int fd, const InetAddress &perrAddr) : IOEvent(owner->GetEventLoop(), fd), m_owner(owner) {
        // 1,初始化buffer
        m_read_buf = new Buffer();
        m_write_buf = new Buffer();

        // 2,初始化链接名称
        auto ip_port = perrAddr.SerializationToIpPort();
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        m_name = std::to_string(timestamp) + "_" + ip_port;

        // 3,设置状态
        SocketOpt::SetKeepAlive(fd, true);
        m_status = ConnStatus::CONNECTING;
    }

    ~TcpConn() override {
        // 避免释放连接时候内存泄漏
        if (m_read_buf) {
            delete m_read_buf;
        }
        if (m_write_buf) {
            delete m_write_buf;
        }
    }

 public:
    void SendData(const char *data, size_t data_size);
    void SendData(const std::string &data);
    Buffer &GetReadBuf() { return *m_read_buf; }
    Buffer &GetWriteBuf() { return *m_write_buf; }
    std::string GetConnName() { return m_name; }
    void SetStatus(ConnStatus status) { m_status = status; }

 public:
    void ProcessWriteEvent() override;
    void ProcessReadEvent() override;

 private:
    ConnOwner *m_owner;  // tcpclient或tcpserver会传入this指针，此时owner_指向tcpclient或tcpserver
    Buffer *m_read_buf;
    Buffer *m_write_buf;
    std::string m_name;  // 连接的name
    ConnStatus m_status;
};

}  // namespace EasyNet

#endif