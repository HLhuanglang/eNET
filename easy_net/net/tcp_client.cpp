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
    m_connector->Start();
}

void TcpClient::NewConn(int fd, const InetAddress &peerAddr) {
    m_conn = std::make_shared<TcpConn>(this, fd, peerAddr);
    m_conn->SetStatus(TcpConn::ConnStatus::CONNECTED);
    m_conn->EnableRead();
    if (onNewConnection != nullptr) {
        onNewConnection(m_conn);
    }
}

void TcpClient::DelConn(const TcpConnSPtr &conn) {
    if (onDelConnection != nullptr) {
        onDelConnection(conn);
    }
    conn->RemoveEvent();
}

void TcpClient::RecvMsg(const TcpConnSPtr &conn) {
    if (onRecvMsg != nullptr) {
        onRecvMsg(conn);
    }
}

void TcpClient::WriteComplete(const TcpConnSPtr &conn) {
    if (onWriteComplete != nullptr) {
        onWriteComplete(conn);
    }
}

EventLoop *TcpClient::GetEventLoop() const {
    return m_loop;
}