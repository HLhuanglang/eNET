#include "tcp_server.h"

#include <memory>
#include <string>
#include <utility>

#include "acceptor.h"
#include "def.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "non_copyable.h"
#include "server_thread.h"
#include "tcp_connection.h"

using namespace EasyNet;

CallBack TcpServer::onNewConnection;
CallBack TcpServer::onDelConnection;
CallBack TcpServer::onRecvMsg;
CallBack TcpServer::onWriteComplete;

TcpServer::TcpServer(const std::string &nameArg, unsigned int numEventThreads,
                     const InetAddress &listenAddr)
    : m_loop(new EventLoop(nameArg + "_loop")),
      m_addr(listenAddr),
      m_name(nameArg),
      m_thread_cnt(numEventThreads) {
    m_acceptor = make_unique<Acceptor>(this, listenAddr, true);
}

TcpServer::~TcpServer() {
    for (auto &conn : m_connections_map) {
        conn->RemoveEvent();
    }
    m_connections_map.clear();
}

// 应该先loop，然后再acceptor
void TcpServer::start() {
    // 1,开启子线程(如果是子线程,这里直接跳过)
    startThreadPool();

    // 2,开启主线程
    if (m_acceptor) {
        m_acceptor->StartListen();
    }
    m_loop->Loop();

    // 3,发生错误
    LOG_ERROR("Svr {} error, quit", m_name);
}

void TcpServer::join_thread() {
    for (auto &n : m_child_svr_vec) {
        n->Join();
    }
}

void TcpServer::detach_thread() {
    for (auto &n : m_child_svr_vec) {
        n->Detach();
    }
}

void TcpServer::NewConn(int fd, const InetAddress &peerAddr) {
    auto tcp_conn = std::make_shared<TcpConn>(this, fd, peerAddr);
    m_connections_map.insert(tcp_conn);
    tcp_conn->SetStatus(TcpConn::ConnStatus::CONNECTED);
    tcp_conn->EnableRead();
    if (onNewConnection != nullptr) {
        onNewConnection(tcp_conn);
    }
}

void TcpServer::DelConn(const tcp_connection_t &conn) {
    if (onDelConnection != nullptr) {
        onDelConnection(conn);
    }

    conn->RemoveEvent();
    size_t n = m_connections_map.erase(conn);
    (void)n;
    assert(n == 1);
}

void TcpServer::RecvMsg(const tcp_connection_t &conn) {
    if (onRecvMsg != nullptr) {
        onRecvMsg(conn);
    }
}

void TcpServer::WriteComplete(const tcp_connection_t &conn) {
    if (onWriteComplete != nullptr) {
        onWriteComplete(conn);
    }
}

EventLoop *TcpServer::GetEventLoop() const {
    return m_loop;
}

void TcpServer::startThreadPool() {
    for (int i = 1; i <= m_thread_cnt; i++) {
        std::unique_ptr<ServerThread> svr_thread = make_unique<ServerThread>("child_svr_" + std::to_string(i), this->m_addr);
        auto tmp_loop = svr_thread->StartServerThread();
        m_child_svr_vec.push_back(std::move(svr_thread));
        m_child_loop_vec.push_back(tmp_loop);
    }
}