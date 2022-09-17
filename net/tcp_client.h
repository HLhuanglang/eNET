#ifndef __TCP_CLIENT_H
#define __TCP_CLIENT_H

#include "event_loop.h"
#include <cstddef>

class tcp_client {
public:
    using after_connection_cb_f = std::function<void(tcp_client* cli, void* args)>;

public:
    tcp_client(event_loop* loop, const char* ip, size_t port);
    ~tcp_client();

public:
    //设置当接收到客户端数据时回调
    //---主要用于对服务端发来的数据进程处理
    void set_recv_msg_cb(const recv_msg_cb_f& t);

    //设置连接建立完成后回调
    //---对于客户端而言,这个回调是客户端发送数据的入口(只有知道什么时候连接建立好了,什么时候发送)。发送的操作,应该封装在channel中
    void set_build_connection_cb(const after_connection_cb_f& f);

    //设置断开连接回调
    //---一般用于服务器断开或者客户端断开后打印信息
    void set_close_connection_cb(std::function<void()>);

    //发送数据接口
    //---这个接口不应该暴露在外面,因为不允许单独调用... 只能从连接回调中进行触发
    void send_data(const char* data, size_t size);

    //运行
    void start() { loop_->process_event(); }

private:
    event_loop* loop_;
};

#endif