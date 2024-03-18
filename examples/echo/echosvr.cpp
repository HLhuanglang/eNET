#include "config.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include <iostream>

int main() {
    EasyNet::EventLoop loop; // 1，创建epollfd、eventfd
    EasyNet::InetAddress addr("127.0.0.1", 8888);
    EasyNet::TcpServer svr(4, addr, "tcpsvr-demo", true, &loop); // 2，创建socketfd、idlefd

    svr.set_new_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        std::cout << "Get New Conn" << std::endl;
    });

    svr.set_recv_msg_cb([](const EasyNet::tcp_connection_t &conn) {
        auto msg = conn->GetReadBuf().RetriveAllAsString();
        std::cout << "msg=" << msg << std::endl;
        conn->SendData(msg);
    });

    svr.set_del_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        std::cout << "Remove conn: " << conn->GetConnName() << std::endl;
    });

    svr.set_write_complete_cb([](const EasyNet::tcp_connection_t &conn) {
        std::cout << "Sent Complete: " << conn->GetConnName() << std::endl;
    });

    svr.start();
}