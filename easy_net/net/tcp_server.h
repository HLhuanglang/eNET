/*
 server主线程应该只做监听的动作，具体数据收发应该由subreactor来做。
*/
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H

#include <cstddef>
#include <map>

#include "event_loop.h"
#include "subreactor_pool.h"

#include "buffer.h"
#include "cb.h"

class tcp_connection;
class tcp_server {
public:
    tcp_server(event_loop *loop, const char *ip, size_t port, int thread_cnt);
    ~tcp_server() = default;

public:
    //设置当接收到客户端数据时回调
    //---接收客户端的数据,做业务逻辑.
    void set_recv_msg_cb(recv_msg_cb_f t);

    //设置连接建立完成后回调
    //---一般用于打印一下对端信息
    void set_build_connection_cb(std::function<void()>);

    //设置断开连接回调
    //---一般用于服务器断开或者客户端断开后打印信息
    void set_close_connection_cb(std::function<void()>);

    //设置loop中的线程数量,必须在start之前设置.
    // void set_thread_cnt(size_t cnt);

    //运行
    void start() { loop_->process_event(); }

private:
    void _do_accept();

private:
    event_loop *loop_; //每一个tcp服务器都应该有一个loop,用来处理各种事件
    int socketfd_;     //处理监听请求
    int idlefd_;       //占位符,防止文件描述符使用达到上限导致无法处理连接事件
    subreactor_pool *sub_reactor_pool_;
};

#endif