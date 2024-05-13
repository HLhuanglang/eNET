#ifndef __EASYNET_TCP_CLIENT_H
#define __EASYNET_TCP_CLIENT_H

#include <memory>

#include "connection_owner.h"
#include "def.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"

namespace EasyNet {
// 前置声明
class Connector;

class TcpClient : public ConnOwner {
 public:
    TcpClient(EventLoop *loop, const InetAddress &addr);
    ~TcpClient();

 public:
    // 框架本身需要关心的接口
    void NewConn(int fd, const InetAddress &peerAddr) override;
    void DelConn(const tcp_connection_t &conn) override;
    void RecvMsg(const tcp_connection_t &conn) override;
    void WriteComplete(const tcp_connection_t &conn) override;
    EventLoop *GetEventLoop() const override;

 public:
    // tcp_client使用者需要关心的接口，由框架回调
    CallBack onNewConnection;
    CallBack onDelConnection;
    CallBack onRecvMsg;
    CallBack onWriteComplete;

 public:
    void connect();

 private:
    EventLoop *m_loop;
    std::unique_ptr<Connector> m_connector;
    tcp_connection_t m_conn;  // 必须持有一下,不然NewConn结束链接就释放了
};
}  // namespace EasyNet

#endif