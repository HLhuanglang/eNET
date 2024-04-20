#include "tcp_client.h"
#include "connector.h"

using namespace EasyNet;

TcpClient::TcpClient(EventLoop *loop, const InetAddress &addr)
    : m_loop(loop) {
    m_connector = make_unique<Connector>(this, addr);
}