# EasyNet开发日志

## 2022年9月17日
今天从较高的层面来看怎么设计api，站在一个库的使用者角度。初步的样子：

服务端：
``` cpp
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
```
客户端：
``` cpp
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
    不可能每次想发送消息，都得重新创建一个tcp_client，然后设置一大堆操作回调，这样会显得很呆
    而且代码也不好实现.
*/
```