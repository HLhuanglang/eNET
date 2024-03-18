#include "event_loop.h"
#include "inet_addr.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include <iostream>

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%D %H:%M:%S.%e][%L][pid %t] %^%v%$");

    EasyNet::EventLoop loop; // 1，创建epollfd、eventfd
    EasyNet::InetAddress addr("127.0.0.1", 8888);
    EasyNet::TcpServer svr(4, addr, "tcpsvr-demo", true, &loop); // 2，创建socketfd、idlefd

    svr.set_new_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        spdlog::debug("Get New Conn");
    });

    svr.set_recv_msg_cb([](const EasyNet::tcp_connection_t &conn) {
        auto msg = conn->GetReadBuf().RetriveAllAsString();
        spdlog::debug("recvMsg={}", msg);
        conn->SendData(msg);
    });

    svr.set_del_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        spdlog::debug("Remove Conn:{}", conn->GetConnName());
    });

    svr.set_write_complete_cb([](const EasyNet::tcp_connection_t &conn) {
        spdlog::debug("Sent Complete: {}", conn->GetConnName());
    });

    svr.start();
}