#include "tcp_server.h"
#include "acceptor.h"
#include "inet_addr.h"
#include "log.h"
#include "non_copyable.h"
#include "socket_opt.h"
#include "tcp_connection.h"
#include <memory>
#include <string>
#include <utility>

using namespace EasyNet;

TcpServer::TcpServer(unsigned int numEventThreads,
                     const InetAddress &listenAddr,
                     const std::string &nameArg,
                     bool isReusePort, EventLoop *pMainLoop)
    : m_main_loop(pMainLoop),
      m_addr(listenAddr),
      m_name(nameArg),
      m_thread_cnt(numEventThreads) {
    if (pMainLoop != NULL) {
        m_acceptor = make_unique<Acceptor>(this, listenAddr, isReusePort);
    }
}

TcpServer::~TcpServer() {
    for (auto &conn : m_connections_map) {
        conn.second->RemoveEvent();
    }
    m_connections_map.clear();
}

void TcpServer::start(int thread_cnt) {
    // 1,开启监听
    m_acceptor->StartListen();

    // 2, LOOP
    m_main_loop->Loop();
}

void TcpServer::NewConn(int fd, const InetAddress &peerAddr) {
    LOG_DEBUG("New Conn, ip:port=%s", peerAddr.SerializationToIpPort().c_str());
    auto tcp_conn = std::make_shared<TcpConn>(this, fd, peerAddr);
    m_connections_map[tcp_conn->GetConnName()] = tcp_conn;
    tcp_conn->EnableRead();
    if (m_new_connection_cb != nullptr) {
        m_new_connection_cb(tcp_conn);
    }
}

void TcpServer::DelConn(const tcp_connection_t &conn) {
    if (m_del_connection_cb != nullptr) {
        m_del_connection_cb(conn);
    }
    conn->RemoveEvent();
    m_connections_map.erase(conn->GetConnName());
}

void TcpServer::RecvMsg(const tcp_connection_t &conn) {
    if (m_revc_msg_cb != nullptr) {
        m_revc_msg_cb(conn);
    }
}

void TcpServer::WriteComplete(const tcp_connection_t &conn) {
    if (m_write_complete_cb != nullptr) {
        m_write_complete_cb(conn);
    }
}

EventLoop *TcpServer::GetEventLoop() const {
    return m_main_loop;
}