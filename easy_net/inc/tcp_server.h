/*
 tcp_server在main函数中创建运行，也就是说一整个服务程序都是tcp_server的资产
 要考虑到所有的回调设置来源都是tcp_server传入的，设置完回调后，如何从main传递
 到子线程中去。
*/
#ifndef __EASYNET_TCP_SERVER_H
#define __EASYNET_TCP_SERVER_H

#include <cstddef>
#include <map>
#include <thread>

#include "connection_owner.h"
#include "event_loop.h"
#include "subreactor_pool.h"

#include "acceptor.h"
#include "buffer.h"
#include "cb.h"

class tcp_connection;
class tcp_server : public connection_owner {
public:
    tcp_server(event_loop *loop, const char *ip, size_t port);
    ~tcp_server() = default;

public:
    virtual void new_connection(int fd, std::string ip, std::string port) override;
    virtual void del_connection(const sp_tcp_connectopn_t &conn) override;
    virtual void recv_msg(const sp_tcp_connectopn_t &conn, buffer &buf) override;
    virtual void write_complete(const sp_tcp_connectopn_t &conn) override;
    virtual void high_water_mark(const sp_tcp_connectopn_t &conn, size_t mark) override;
    virtual event_loop *get_loop() const override;

public:
    //设置连接建立完成后回调
    void set_new_connection_cb(std::function<void()>);

    //设置当接收到客户端数据时回调
    void set_recv_msg_cb(recv_msg_cb_f t);

    //设置应用层数据缓冲发送完毕回调
    void set_write_complete_cb();

    //设置断开连接回调
    void set_close_connection_cb(std::function<void()>);

    //运行tcp server，默认情况下线程数为机器核心的2倍
    void start(int thread_cnt = 2 * static_cast<int>(std::thread::hardware_concurrency()));

private:
    void _do_accept();

private:
    event_loop *main_loop_; //每一个tcp服务器都应该有一个loop,用来处理各种事件
    subreactor_pool *sub_reactor_pool_;
    acceptor *acceptor_; //主线程中负责处理链接请求
};

#endif