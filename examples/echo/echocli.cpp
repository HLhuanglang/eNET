#include "event_loop.h"
#include "inet_addr.h"
#include "spdlog/common.h"
#include "tcp_client.h"
#include <iostream>
#include <signal.h>

#include "log.h"

void sighandler(int signum) {
    std::cout << "Bye~" << std::endl;
    exit(1);
}

int main() {
    signal(SIGINT, sighandler);

    // 设置日志
    EasyNet::LogInit(spdlog::level::debug);

    EasyNet::EventLoop loop("echocli_loop");
    EasyNet::TcpClient cli(&loop, {"127.0.0.1", 8888});

    cli.set_new_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Get New Conn");
        std::string send_msg;
        std::cin >> send_msg;
        conn->SendData(send_msg);
    });

    cli.set_recv_msg_cb([](const EasyNet::tcp_connection_t &conn) {
        auto msg = conn->GetReadBuf().RetriveAllAsString();
        LOG_DEBUG("recv msg={}", msg);
        std::string send_msg;
        std::cin >> send_msg;
        conn->SendData(send_msg);
    });

    cli.set_del_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
    });

    cli.set_write_complete_cb([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
    });

    cli.connect();
    loop.Loop();
}