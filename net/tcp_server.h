/*
    服务器不应该具备主动发送数据的能力，而应该是在客户端请求后被动响应发送
 */
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H

#include "buffer.h"
#include "cb.h"
#include "event_loop.h"
#include <cstddef>
#include <map>

class tcp_connection;
class tcp_server {
public:
    tcp_server(event_loop* loop, const char* ip, size_t port);
    ~tcp_server();

public:
    //设置当接收到客户端数据时回调
    //---接收客户端的数据,做业务逻辑.
    void set_recv_msg_cb(const recv_msg_cb_f& t);

    //设置连接建立完成后回调
    //---一般用于打印一下对端信息
    void set_build_connection_cb(std::function<void()>);

    //设置断开连接回调
    //---一般用于服务器断开或者客户端断开后打印信息
    void set_close_connection_cb(std::function<void()>);

    //设置loop中的线程数量
    void set_thread_cnt(size_t cnt);

    //运行
    void start() { loop_->process_event(); }

private:
    event_loop* loop_;                              //每一个tcp服务器都应该有一个loop,用来处理各种事件
    std::map<int, tcp_connection*> connection_map_; // acceptfd----connection：通过acceptfd来定位到具体的连接
};

#endif