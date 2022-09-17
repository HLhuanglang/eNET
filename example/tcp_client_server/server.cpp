#include "../../net/easy_net.h"

#include <iostream>

int main() {
    event_loop loop;
    tcp_server server(&loop, "127.0.0.1", 8888);

    // 1,设置连接建立回调
    server.set_build_connection_cb([]() {
        // 1,1打印日志
        std::cout << "connection build!" << std::endl;
        // 1,2记录对端信息
    });

    // 2,设置断开连接回调
    server.set_close_connection_cb([]() {
        // 2,1打印日志
        std::cout << "connection close!" << std::endl;
        // 2,2记录对端信息
    });

    // 3,设置消息处理回调
    server.set_recv_msg_cb([&](tcp_connection* conn, buffer* buf) {
        // 3,1 处理数据：todo handle buf
        // 2.2 回复客户端
        std::string ret = "hello,client!";
        conn->send_data(ret.c_str(), ret.size());
    });

    // 4,启动
    server.set_thread_cnt(4);
    server.start();
}