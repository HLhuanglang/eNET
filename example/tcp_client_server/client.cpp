#include "../../net/easy_net.h"

#include <iostream>

int main() {
    event_loop loop;
    tcp_client client(&loop, "127.0.0.1", 8888);

    // 1,设置连接建立回调
    client.set_build_connection_cb([](tcp_client* cli, void* args) {
        //客户端主动发送数据的地方!
        std::string resp = "hello,server!";
        cli->send_data(resp.c_str(), resp.size());
    });

    // 2,设置连接断开回调
    client.set_close_connection_cb([]() {
        // 2,1打印日志
        std::cout << "connection close!" << std::endl;
        // 2,2记录对端信息
    });

    // 3,设置消息处理回调
    client.set_recv_msg_cb([](tcp_connection* conn, buffer* buf) {
        // 3.1 处理数据：todo handle buf
    });

    // 4,启动
    client.start();
}

/*
    上面这种接口模式,如果client想在任意逻辑中发送消息,那怎么实现？

    不可能每次想发送消息，都得重新定义tcp_client，然后设置上面这一大堆操作把
*/