#ifndef __EASYNET_TCP_CLIENT_H
#define __EASYNET_TCP_CLIENT_H

#include <cstddef>

#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"

class tcp_client {
 public:
    tcp_client(event_loop *loop, const inet_addr &addr);
    ~tcp_client();

 public:
    //设置连接建立完成后回调
    void set_new_connection_cb(new_connection_cb_f cb) {
        m_new_connection_cb = std::move(cb);
    }

    void set_del_connection_cb(del_connection_cb_f cb) {
        m_del_connection_cb = std::move(cb);
    }

    //设置当接收到客户端数据时回调
    void set_recv_msg_cb(recv_msg_cb_f cb) {
        m_revc_msg_cb = std::move(cb);
    }

    //设置应用层数据缓冲发送完毕回调
    void set_write_complete_cb(write_complete_cb_f cb) {
        m_write_complete_cb = std::move(cb);
    }

    void set_high_water_mark_cb(high_water_mark_cb_f cb) {
        m_high_water_mark_cb = std::move(cb);
    }

    //运行
    void start() { m_loop->loop(); }

 private:
    new_connection_cb_f m_new_connection_cb;
    del_connection_cb_f m_del_connection_cb;
    recv_msg_cb_f m_revc_msg_cb;
    write_complete_cb_f m_write_complete_cb;
    high_water_mark_cb_f m_high_water_mark_cb;

 private:
    event_loop *m_loop;
};

#endif