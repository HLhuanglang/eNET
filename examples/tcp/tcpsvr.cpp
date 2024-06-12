#include <signal.h>

#include <atomic>
#include <iostream>
#include <thread>

#include "inet_addr.h"
#include "log.h"
#include "tcp_server.h"

std::atomic<int> global_counter(0);

void sighandler(int signum) {
    printf("\nCaught signal %d, coming out...\n", signum);
    std::cout << "global_counter=" << global_counter << std::endl;
    exit(1);
}

int main() {
    signal(SIGINT, sighandler);

    // 设置日志
    EasyNet::LogInit(EasyNet::level::debug);

    // 创建tcpsvr
    EasyNet::TcpServer svr("tcpsvr", 2 * std::thread::hardware_concurrency() - 1, {"127.0.0.1", 8888});

    // 设置业务回调
    svr.onNewConnection = ([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("{}: Get New Conn", conn->GetConnName());
    });

    svr.onRecvMsg = ([](const EasyNet::tcp_connection_t &conn) {
        auto msg = conn->GetBuffer().RetriveAllAsString();
        if (msg.empty()) {
            global_counter.fetch_add(1, std::memory_order_relaxed);
        } else {
            LOG_DEBUG("{}: msg={}", conn->GetConnName(), msg);
            conn->SendData(msg);
        }
    });

    svr.onDelConnection = ([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("{}: Remove Conn", conn->GetConnName());
    });

    svr.onWriteComplete = ([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("{}: Sent Complete", conn->GetConnName());
    });

    // 启动服务
    svr.start();
}