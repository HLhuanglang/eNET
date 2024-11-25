#ifndef __EASYNET_CONNECTOR_H
#define __EASYNET_CONNECTOR_H

#include "inet_addr.h"
#include "io_event.h"
#include "tcp_client.h"

namespace EasyNet {
class Connector : public IOEvent {
 public:
    Connector(TcpClient *client, const InetAddress &addr)
        : IOEvent(client->GetEventLoop(), -1),  // Connector本身不持有fd
          m_client(client),
          m_status(ConnectState::DISCONNECTED) {
        m_addr = addr;
        m_retry_delay_ms = KInitRetryTimeMS;
    }

    ~Connector() = default;

 public:
    void ProcessWriteEvent() override;

 public:
    // connector负责连接对端
    void Start();  // 开始主动发起连接

 private:
    void Retry();
    void ReConnect();

 private:
    enum class ConnectState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        CONNERROR,
    };
    ConnectState m_status;  // 当前连接状态
    InetAddress m_addr;     // 服务端地址
    TcpClient *m_client;    // 当前connector属于哪一个TcpClient,生命周期由TcpClient控制
    int m_retry_delay_ms;   // 重试时间
};
}  // namespace EasyNet

#endif  // !__EASYNET_CONNECTOR_H
