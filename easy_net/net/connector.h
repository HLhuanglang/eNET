#ifndef __EASYNET_CONNECTOR_H
#define __EASYNET_CONNECTOR_H

#include "inet_addr.h"
#include "io_event.h"
#include "tcp_client.h"

namespace EasyNet {
class Connector : public IOEvent {
 public:
    Connector(TcpClient *client, const InetAddress &addr)
        : IOEvent(client->GetEventLoop(), -1),
          m_client(client) {
        m_addr = addr;
    }

    ~Connector() = default;

 public:
 private:
    InetAddress m_addr;
    TcpClient *m_client; // 当前acceptor属于哪一个TcpServer,生命周期由TcpServer控制
};
} // namespace EasyNet

#endif // !__EASYNET_CONNECTOR_H
