#include "event_loop.h"
#include "inet_addr.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include <atomic>
#include <iostream>
#include <signal.h>
#include <thread>

#include "log.h"

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
    EasyNet::InetAddress addr("127.0.0.1", 8888);
    EasyNet::TcpServer svr("tcpsvr-demo", 2 * std::thread::hardware_concurrency(), addr);

    // 设置业务回调
    svr.set_new_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Get New Conn");
    });

    svr.set_recv_msg_cb([](const EasyNet::tcp_connection_t &conn) {
        // fixme-hl：出现回包为空的情况
        auto msg = conn->GetReadBuf().RetriveAllAsString();
        if (msg.empty()) {
            global_counter.fetch_add(1, std::memory_order_relaxed);
        } else {
            LOG_DEBUG("msg={}", msg);
            conn->SendData(msg);
        }
    });

    svr.set_del_connection_cb([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
    });

    svr.set_write_complete_cb([](const EasyNet::tcp_connection_t &conn) {
        LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
    });

    // 启动服务
    svr.start();
}