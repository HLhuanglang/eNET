/*
 tcp_server在main函数中创建运行，也就是说一整个服务程序都是tcp_server的资产
 要考虑到所有的回调设置来源都是tcp_server传入的，设置完回调后，如何从main传递
 到子线程中去。
*/
#ifndef __EASYNET_TCP_SERVER_H
#define __EASYNET_TCP_SERVER_H

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "connection_owner.h"
#include "def.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"

namespace EasyNet {

// 前置声明
class TcpConn;
class Acceptor;
class ServerThread;

class TcpServer : public ConnOwner {
 public:
    TcpServer(const std::string &svrname, unsigned int numEventThreads, const InetAddress &listenAddr);
    ~TcpServer();

 public:
    // 框架本身需要关心的接口
    void NewConn(int fd, const InetAddress &peerAddr) override;
    void DelConn(const tcp_connection_t &conn) override;
    void RecvMsg(const tcp_connection_t &conn) override;
    void WriteComplete(const tcp_connection_t &conn) override;
    EventLoop *GetEventLoop() const override;

 public:
    // tcp_server使用者需要关心的接口，由框架回调
    static CallBack onNewConnection;
    static CallBack onDelConnection;
    static CallBack onRecvMsg;
    static CallBack onWriteComplete;

 public:
    // 运行tcp server
    void start();

    // join/detach 子线程
    void join_thread();
    void detach_thread();

    // 获取loop
    EventLoop *get_loop();

 private:
    void startThreadPool();

 private:
    EventLoop *m_loop;                                           // 每一个tcp服务器都应该有一个loop,用来处理各种事件
    std::unique_ptr<Acceptor> m_acceptor;                        // 主线程中负责处理链接请求
    InetAddress m_addr;                                          // 服务器监听的地址
    std::string m_name;                                          // 服务器名称
    unsigned int m_thread_cnt;                                   // 服务器启动的线程数量
    std::set<std::shared_ptr<TcpConn>> m_connections_map;        // 当前持有的tcp链接[防止智能指针被释放]
    std::vector<std::unique_ptr<ServerThread>> m_child_svr_vec;  // 子线程
    std::vector<EventLoop *> m_worker_loop;                      // 工作线程loop
};
}  // namespace EasyNet

#endif