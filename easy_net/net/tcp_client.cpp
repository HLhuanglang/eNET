#include "tcp_client.h"

#include "connector.h"

using namespace EasyNet;

TcpClient::TcpClient(EventLoop *loop, const InetAddress &addr)
    : m_loop(loop) {
    m_connector = make_unique<Connector>(this, addr);
}
TcpClient::~TcpClient() {
    // 放在头文件会因为Connector类只是前置申明导致unique_ptr断言失败
}

void TcpClient::connect() {
    m_connector->Connect();
}

void TcpClient::NewConn(int fd, const InetAddress &peerAddr) {
    m_conn = std::make_shared<TcpConn>(this, fd, peerAddr);
    m_conn->SetStatus(TcpConn::ConnStatus::CONNECTED);
    m_conn->EnableRead();
    if (m_new_connection_cb != nullptr) {
        m_new_connection_cb(m_conn);
    }
}

void TcpClient::DelConn(const tcp_connection_t &conn) {
    if (m_del_connection_cb != nullptr) {
        m_del_connection_cb(conn);
    }
}

void TcpClient::RecvMsg(const tcp_connection_t &conn) {
    if (m_revc_msg_cb != nullptr) {
        m_revc_msg_cb(conn);
    }
}

void TcpClient::WriteComplete(const tcp_connection_t &conn) {
    if (m_write_complete_cb != nullptr) {
        m_write_complete_cb(conn);
    }
}

EventLoop *TcpClient::GetEventLoop() const {
    return m_loop;
}