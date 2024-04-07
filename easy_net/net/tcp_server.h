/*
 tcp_server在main函数中创建运行，也就是说一整个服务程序都是tcp_server的资产
 要考虑到所有的回调设置来源都是tcp_server传入的，设置完回调后，如何从main传递
 到子线程中去。
*/
#ifndef __EASYNET_TCP_SERVER_H
#define __EASYNET_TCP_SERVER_H

#include <cstddef>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "connection_owner.h"
#include "def.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"

#include "buffer.h"

namespace EasyNet {

// 前置声明
class TcpConn;
class Acceptor;
class ServerThread;

class TcpServer : public ConnOwner {
 public:
    /// numEventThreads:需要创建的IO线程数量
    /// 如果numEventThreads = 0, 则要求pMainLoop != NULL
    /// 如果numEventThreads > 0, 则启动n个线程并用多个套接字并发监听同一个端口
    TcpServer(unsigned int numEventThreads, const InetAddress &listenAddr,
              const std::string &nameArg, bool isReusePort,
              EventLoop *pMainLoop);

    ~TcpServer();

    // 框架本身需要关心的接口
 public:
    void NewConn(int fd, const InetAddress &peerAddr) override;
    void DelConn(const tcp_connection_t &conn) override;
    void RecvMsg(const tcp_connection_t &conn) override;
    void WriteComplete(const tcp_connection_t &conn) override;
    EventLoop *GetEventLoop() const override;

    // tcp_server使用者需要关心的接口，由框架回调
 public:
    // 设置连接建立完成后回调
    void set_new_connection_cb(CallBack cb) {
        m_new_connection_cb = cb;
    }

    // 设置连接被删除通知回调
    void set_del_connection_cb(CallBack cb) {
        m_del_connection_cb = cb;
    }

    // 设置当接收到客户端数据时回调
    void set_recv_msg_cb(CallBack cb) {
        m_revc_msg_cb = cb;
    }

    // 设置应用层数据缓冲发送完毕回调
    void set_write_complete_cb(CallBack cb) {
        m_write_complete_cb = cb;
    }

    // 运行tcp server
    void start();

    // join/detach 子线程
    void join_thread();
    void detach_thread();

 private:
    void startThreadPool();

 public:
    static CallBack m_new_connection_cb;
    static CallBack m_del_connection_cb;
    static CallBack m_revc_msg_cb;
    static CallBack m_write_complete_cb;

 private:
    EventLoop *m_loop;                                          // 每一个tcp服务器都应该有一个loop,用来处理各种事件
    std::unique_ptr<Acceptor> m_acceptor;                       // 主线程中负责处理链接请求
    InetAddress m_addr;                                         // 服务器监听的地址
    std::string m_name;                                         // 服务器名称
    unsigned int m_thread_cnt;                                  // 服务器启动的线程数量
    std::set<std::shared_ptr<TcpConn>> m_connections_map;       // 当前持有的tcp链接
    std::vector<std::unique_ptr<ServerThread>> m_child_svr_vec; // 子线程
    std::vector<EventLoop *> m_child_loop_vec;                  // 子线程的loop
};
} // namespace EasyNet

#endif