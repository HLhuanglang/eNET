// 测试：telnet 127.0.0.1 12345

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

const int k_max_threads = 4;
const int k_port = 12345;

void worker(int sock) {
    char buf[1024];
    while (true) {
        int n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) {
            break;
        }
        std::cout << "Recv:" << buf << std::endl;
        send(sock, buf, n, 0);
    }
    close(sock);
}

int main() {
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    int optval = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        std::cerr << "Failed to set SO_REUSEPORT option" << std::endl;
        return -1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(k_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << k_port << std::endl;
        return -1;
    }

    if (listen(listen_sock, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -1;
    }

    std::vector<std::thread> threads;
    threads.reserve(k_max_threads);
    for (int i = 0; i < k_max_threads; ++i) {
        threads.emplace_back([i, &listen_sock]() {
            while (true) {
                std::cout << "waiting..." << i << std::endl;
                int sock = accept(listen_sock, nullptr, nullptr);
                std::cout << "working..." << i << std::endl;
                if (sock < 0) {
                    break;
                }
                worker(sock);
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    close(listen_sock);
    return 0;
}