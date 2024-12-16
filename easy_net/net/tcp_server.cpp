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

EventCallBack TcpServer::onNewConnection;
EventCallBack TcpServer::onDelConnection;
EventCallBack TcpServer::onRecvMsg;
EventCallBack TcpServer::onWriteComplete;

TcpServer::TcpServer(const std::string &nameArg, unsigned int numEventThreads,
                     const InetAddress &listenAddr)
    : m_loop(new EventLoop(nameArg + "_loop")),
      m_addr(listenAddr),
      m_name(nameArg),
      m_thread_cnt(numEventThreads) {
    m_acceptor = make_unique<Acceptor>(this, listenAddr, true);
    m_worker_loop.push_back(m_loop);
}

TcpServer::~TcpServer() {
    for (auto &conn : m_connections_map) {
        conn->RemoveEvent();
    }
    m_connections_map.clear();
    m_worker_loop.clear();

    // 释放资源：析构顺序：自身(先处理成员变量析构,再处理自身)->父类
    if (m_loop != nullptr) {
        delete m_loop;
        m_loop = nullptr;
    }
}

// 应该先loop，然后再acceptor
void TcpServer::start() {
    // 1,开启子线程(如果是子线程,这里直接跳过,由m_thread_cnt来判断)
    startThreadPool();

    // 2,开启主线程
    if (m_acceptor) {
        m_acceptor->StartListen();
    }
    m_loop->Loop();

    LOG_ERROR("Svr {} quit", m_name);
}

void TcpServer::stop() {
    // 先停止子线程
    for (auto &loop : m_worker_loop) {
        if (loop == m_loop) {
            continue;
        }
        loop->Quit();
    }
    // 等子线程停止后,再停止主线程
    JoinThread();
    m_loop->Quit();
}

void TcpServer::JoinThread() {
    for (auto &n : m_child_svr_vec) {
        n->Join();
    }
}

void TcpServer::DetachThread() {
    for (auto &n : m_child_svr_vec) {
        n->Detach();
    }
}

EventLoop *TcpServer::get_loop() {
    static int idx = 0;
    if (idx == m_thread_cnt + 1) {
        idx = 0;
    }
    return m_worker_loop[idx++ % (m_thread_cnt + 1)];
}

EventLoop *TcpServer::get_main_loop() {
    return m_loop;
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

void TcpServer::DelConn(const TcpConnSPtr &conn) {
    if (onDelConnection != nullptr) {
        onDelConnection(conn);
    }

    conn->RemoveEvent();
    size_t n = m_connections_map.erase(conn);
    (void)n;
    assert(n == 1);
}

void TcpServer::RecvMsg(const TcpConnSPtr &conn) {
    if (onRecvMsg != nullptr) {
        onRecvMsg(conn);
    }
}

void TcpServer::WriteComplete(const TcpConnSPtr &conn) {
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
        m_worker_loop.push_back(tmp_loop);
    }
}