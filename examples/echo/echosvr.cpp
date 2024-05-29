#include <signal.h>
#include <spdlog/common.h>

#include <atomic>
#include <iostream>
#include <thread>

#include "inet_addr.h"
#include "log.h"
#include "tcp_server.h"

// 后缀的参数只能是unsigned long long、long double、const char*或者const char* + size_t
unsigned long long operator"" _s(unsigned long long s) {
    return s * 1000;
}

unsigned long long operator"" _ms(unsigned long long ms) {
    return ms;
}

std::atomic<int> global_counter(0);

void sighandler(int signum) {
    printf("\nCaught signal %d, coming out...\n", signum);
    std::cout << "global_counter=" << global_counter << std::endl;
    exit(1);
}

int main() {
    signal(SIGINT, sighandler);

    // 设置日志
    EasyNet::LogInit(spdlog::level::debug);

    // 创建tcpsvr
    EasyNet::TcpServer svr("echosvr", 2 * std::thread::hardware_concurrency() - 1, {"127.0.0.1", 8888});

    // 设置业务回调
    svr.onNewConnection = ([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Get New Conn");
        conn->GetOwnerLoop()->TimerAfter([=]() {
            LOG_DEBUG("TimerAfter 1s: loop={}", conn->GetOwnerLoop()->GetLoopName());
        },
                                         1_s);
    });

    svr.onRecvMsg = ([](const EasyNet::tcp_connection_t &conn) {
        // fixme-hl：出现回包为空的情况
        auto msg = conn->GetReadBuf().RetriveAllAsString();
        if (msg.empty()) {
            global_counter.fetch_add(1, std::memory_order_relaxed);
        } else {
            LOG_DEBUG("msg={}", msg);
            conn->SendData(msg);
        }
    });

    svr.onDelConnection = ([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
    });

    svr.onWriteComplete = ([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
    });

    // 启动服务
    svr.start();
}