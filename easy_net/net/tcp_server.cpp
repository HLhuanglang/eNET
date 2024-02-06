#include "tcp_server.h"
#include "acceptor.h"
#include "inet_addr.h"
#include "log.h"
#include "non_copyable.h"
#include "socket_opt.h"
#include "tcp_connection.h"
#include <memory>
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
        m_acceptor = make_unique<Acceptor>(this, listenAddr, true);
    }
}

void TcpServer::start(int thread_cnt) {
    // todo
}

void TcpServer::NewConn(int fd, const InetAddress &peerAddr) {
    LOG_DEBUG("New Conn, ip=%s port=%s", peerAddr.SerializationToIP().c_str(), peerAddr.SerializationToIP().c_str());
    auto tcp_conn = std::make_shared<TcpConn>(this, fd);
    m_connections_map["TODO"] = tcp_conn;
    tcp_conn->EnableRead();
    if (m_new_connection_cb != nullptr) {
        m_new_connection_cb(tcp_conn);
    }
}

void TcpServer::DelConn(const sp_tcp_connectopn_t &conn) {
    // todo
}

void TcpServer::RecvMsg(const sp_tcp_connectopn_t &conn) {
    if (m_revc_msg_cb != nullptr) {
        m_revc_msg_cb(conn);
    }
}

void TcpServer::WriteComplete(const sp_tcp_connectopn_t &conn) {
    if (m_write_complete_cb != nullptr) {
        m_write_complete_cb(conn);
    }
}

void TcpServer::HighWaterMark(const sp_tcp_connectopn_t &conn, size_t mark) {
    if (m_high_water_mark_cb != nullptr) {
        m_high_water_mark_cb(conn, mark);
    }
}

EventLoop *TcpServer::GetEventLoop() const {
    return m_main_loop;
}