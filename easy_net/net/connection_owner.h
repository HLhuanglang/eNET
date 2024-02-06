#ifndef __EASYNET_CONNECTION_OWNER_H
#define __EASYNET_CONNECTION_OWNER_H

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

#include "buffer.h"
#include "inet_addr.h"

namespace EasyNet {

// 前置声明
class TcpConn;
class EventLoop;

using sp_tcp_connectopn_t = std::shared_ptr<TcpConn>;

class ConnOwner {
 public:
    ConnOwner() = default;
    virtual ~ConnOwner() = default;

 public:
    // 新建连接
    virtual void NewConn(int fd, const InetAddress &peerAddr) = 0;

    // 断开连接
    virtual void DelConn(const sp_tcp_connectopn_t &conn) = 0;

    // 收到消息
    virtual void RecvMsg(const sp_tcp_connectopn_t &conn) = 0;

    // 发送缓冲区数据已写入内核(不一定发送到对端)
    virtual void WriteComplete(const sp_tcp_connectopn_t &conn) = 0;

    // 发送缓冲区高水位
    virtual void HighWaterMark(const sp_tcp_connectopn_t &conn, size_t mark) = 0;

    // 获取当前连接属于哪个loop
    virtual EventLoop *GetEventLoop() const = 0;
};
} // namespace EasyNet

#endif