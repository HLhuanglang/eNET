#ifndef __EASYNET_CONNECTION_OWNER_H
#define __EASYNET_CONNECTION_OWNER_H

#include "def.h"
#include "inet_addr.h"

namespace EasyNet {

// 前置声明
class EventLoop;

class ConnOwner {
 public:
    ConnOwner() = default;
    virtual ~ConnOwner() = default;

 public:
    // 新建连接
    virtual void NewConn(int fd, const InetAddress &peerAddr) = 0;

    // 断开连接
    virtual void DelConn(const TcpConnSPtr &conn) = 0;

    // 收到消息
    virtual void RecvMsg(const TcpConnSPtr &conn) = 0;

    // 发送缓冲区数据已写入内核(不一定发送到对端)
    virtual void WriteComplete(const TcpConnSPtr &conn) = 0;

    // 获取当前连接属于哪个loop
    virtual EventLoop *GetEventLoop() const = 0;
};
}  // namespace EasyNet

#endif