#include <signal.h>

#include <iostream>

#include "event_loop.h"
#include "inet_addr.h"
#include "log.h"
#include "tcp_client.h"

void sighandler(int signum) {
    std::cout << "Bye~" << std::endl;
    exit(1);
}

int main() {
    signal(SIGINT, sighandler);

    // 设置日志
    EasyNet::LogInit(EasyNet::level::debug);

    EasyNet::EventLoop loop("echocli_loop");
    EasyNet::TcpClient cli(&loop, {"127.0.0.1", 8888});

    cli.onNewConnection = ([](const EasyNet::TcpConnSPtr &conn) {
        LOG_DEBUG("Get New Conn");
        std::string send_msg;
        std::cin >> send_msg;
        conn->SendData(send_msg);
    });

    cli.onRecvMsg = ([](const EasyNet::TcpConnSPtr &conn) {
        auto msg = conn->GetBuffer().RetriveAllAsString();
        LOG_DEBUG("recv msg={}", msg);
        std::string send_msg;
        std::cin >> send_msg;
        conn->SendData(send_msg);
    });

    cli.onDelConnection = ([](const EasyNet::TcpConnSPtr &conn) {
        LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
    });

    cli.onWriteComplete = ([](const EasyNet::TcpConnSPtr &conn) {
        LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
    });

    cli.connect();
    loop.Loop();
}