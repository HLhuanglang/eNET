/*
 tcp_server在main函数中创建运行，也就是说一整个服务程序都是tcp_server的资产
 要考虑到所有的回调设置来源都是tcp_server传入的，设置完回调后，如何从main传递
 到子线程中去。
*/
#ifndef __EASYNET_TCP_SERVER_H
#define __EASYNET_TCP_SERVER_H

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "connection_owner.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"

#include "buffer.h"

class tcp_connection;
class acceptor;
class tcp_server : public connection_owner {
 public:
    tcp_server(event_loop *loop, const char *ip, size_t port);
    ~tcp_server() override = default;

    // 框架本身需要关心的接口
 public:
    void new_connection(int fd, std::string ip, std::string port) override;
    void del_connection(const sp_tcp_connectopn_t &conn) override;
    void recv_msg(const sp_tcp_connectopn_t &conn) override;
    void write_complete(const sp_tcp_connectopn_t &conn) override;
    void high_water_mark(const sp_tcp_connectopn_t &conn, size_t mark) override;
    event_loop *get_loop() const override;

    // tcp_server使用者需要关心的接口
 public:
    // 设置连接建立完成后回调
    void set_new_connection_cb(new_connection_cb_f cb) {
        m_new_connection_cb = std::move(cb);
    }

    void set_del_connection_cb(del_connection_cb_f cb) {
        m_del_connection_cb = std::move(cb);
    }

    // 设置当接收到客户端数据时回调
    void set_recv_msg_cb(recv_msg_cb_f cb) {
        m_revc_msg_cb = std::move(cb);
    }

    // 设置应用层数据缓冲发送完毕回调
    void set_write_complete_cb(write_complete_cb_f cb) {
        m_write_complete_cb = std::move(cb);
    }

    void set_high_water_mark_cb(high_water_mark_cb_f cb) {
        m_high_water_mark_cb = std::move(cb);
    }

    // 运行tcp server，默认情况下线程数为机器核心的2倍
    void start(int thread_cnt = 2 * static_cast<int>(std::thread::hardware_concurrency()));

 private:
    new_connection_cb_f m_new_connection_cb;
    del_connection_cb_f m_del_connection_cb;
    recv_msg_cb_f m_revc_msg_cb;
    write_complete_cb_f m_write_complete_cb;
    high_water_mark_cb_f m_high_water_mark_cb;

 private:
    event_loop *m_main_loop; // 每一个tcp服务器都应该有一个loop,用来处理各种事件
    acceptor *m_acceptor;    // 主线程中负责处理链接请求
    inet_addr m_addr;        // 服务器地址
    std::map<std::string, std::shared_ptr<connection_owner>> m_connections_map;
};

#endif