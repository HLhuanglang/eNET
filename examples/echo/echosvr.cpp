#include "event_loop.h"
#include "inet_addr.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include <iostream>
#include <thread>

//后缀的参数只能是unsigned long long、long double、const char*或者const char* + size_t
unsigned long long operator"" _s(unsigned long long s) {
    return s * 1000;
}

unsigned long long operator"" _ms(unsigned long long ms) {
    return ms;
}

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%D %H:%M:%S.%e][%L][pid %t] %^%v%$");

    EasyNet::EventLoop loop; // 1，创建epollfd、eventfd
    EasyNet::InetAddress addr("127.0.0.1", 8888);
    EasyNet::TcpServer svr(2 * std::thread::hardware_concurrency(), addr, "tcpsvr-demo", true, &loop); // 2，创建socketfd、idlefd

    svr.set_new_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        spdlog::debug("Get New Conn");
    });

    svr.set_recv_msg_cb([](const EasyNet::tcp_connection_t &conn) {
        auto msg = conn->GetReadBuf().RetriveAllAsString();
        spdlog::debug("recvMsg={}", msg);
        conn->SendData("svr:" + msg);
    });

    svr.set_del_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        spdlog::debug("Remove Conn:{}", conn->GetConnName());
    });

    svr.set_write_complete_cb([](const EasyNet::tcp_connection_t &conn) {
        spdlog::debug("Sent Complete: {}", conn->GetConnName());
    });

    svr.start();
}