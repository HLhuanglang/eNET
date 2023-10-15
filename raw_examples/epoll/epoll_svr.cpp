/*
    关注epoll et和lt模式下，read的现象
*/
#include <array>
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

const int k_port = 8888;

int main(int argc, char *argv[]) {
    // 选择ET或者LT模式
    if (argc < 2) {
        std::cout << "Usage: epoll_demo [et|lt]" << std::endl;
        exit(EXIT_FAILURE);
    }
    int epoll_type = 0;
    std::string mode = argv[1];
    if (mode == "et") {
        epoll_type = EPOLLET;
    } else if (mode == "lt") {
        epoll_type = 0; // 默认是lt模式
    } else {
        std::cout << "epoll type must be et or lt: " << mode << std::endl;
        exit(EXIT_FAILURE);
    }

    // socket-bind-listen
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socker" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(k_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << k_port << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    // epoll
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        std::cerr << "Failed to create epoll" << std::endl;
        exit(EXIT_FAILURE);
    }
    struct epoll_event ee;
    bzero(&ee, sizeof ee);
    ee.events = EPOLLIN;
    ee.data.fd = server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ee);

    // epoll_wait
    std::array<epoll_event, 1024> read_events;
    while (true) {
        // ET和LT模式：核心是影响读写的处理逻辑
        // ET只会让epoll_wait返回一次
        // LT只要还有数据，就会一直导致epoll_wait返回
        int n = epoll_wait(epfd, read_events.begin(), 1024, -1); // 阻塞等待事件到达
        if (n < 0 && errno != EINTR) {
            std::cout << "epoll_wait error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            // 因为设置成阻塞模式了，所以不会返回0的
        }

        for (int i = 0; i < n; i++) {
            if (read_events[i].data.fd == server_fd) {
                // 处理连接事件
                struct sockaddr s;
                socklen_t len = sizeof(s);
                int client_fd = accept(server_fd, &s, &len);
                if (client_fd < 0) {
                    std::cout << "accpet error: " << strerror(errno) << std::endl;
                    exit(EXIT_FAILURE);
                }
                fcntl(client_fd, F_SETFL, O_NONBLOCK); // 设置成非阻塞
                struct epoll_event ee;
                bzero(&ee, sizeof ee);
                ee.events = EPOLLIN | epoll_type;
                ee.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ee);
            } else {
                // 处理读写事件
                int client_fd = read_events[i].data.fd;
                uint32_t event = read_events[i].events;
                if ((event & EPOLLIN) != 0U) {
                    std::array<char, 2> buf;
                    auto n = read(client_fd, buf.begin(), 2); // 对端发送大于2个字节的数据过来
                    if (n < 0 && errno == EINTR) {
                    } else if (n == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, nullptr);
                        close(client_fd);
                        break;
                    } else {
                        printf("----%c%c\n", buf[0], buf[1]);
                    }
                }
            }
        }
    }
    return 0;
}