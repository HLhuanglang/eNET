#ifndef __EASYNET_TCP_CLIENT_H
#define __EASYNET_TCP_CLIENT_H

#include <cstddef>

#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"

namespace EasyNet {
class tcp_client {
 public:
    tcp_client(EventLoop *loop, const InetAddress &addr);
    ~tcp_client() = default;

 public:
    // 设置连接建立完成后回调
    void set_new_connection_cb(CallBack cb) {
        m_new_connection_cb = std::move(cb);
    }

    void set_del_connection_cb(CallBack cb) {
        m_del_connection_cb = std::move(cb);
    }

    // 设置当接收到客户端数据时回调
    void set_recv_msg_cb(CallBack cb) {
        m_revc_msg_cb = std::move(cb);
    }

    // 设置应用层数据缓冲发送完毕回调
    void set_write_complete_cb(CallBack cb) {
        m_write_complete_cb = std::move(cb);
    }

    // 运行
    void start() { m_loop->Loop(); }

 private:
    CallBack m_new_connection_cb;
    CallBack m_del_connection_cb;
    CallBack m_revc_msg_cb;
    CallBack m_write_complete_cb;

 private:
    EventLoop *m_loop;
};
} // namespace EasyNet

#endif