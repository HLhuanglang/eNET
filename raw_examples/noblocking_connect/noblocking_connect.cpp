// 测试：使用nc启动一个tcp服务端
// nc -v -l 0.0.0.0 3000

#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000
#define SEND_DATA "helloworld"

int main(int argc, char *argv[]) {
    // 1.创建一个socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        std::cout << "create client socket error." << std::endl;
        return -1;
    }

    // 连接成功以后，我们再将 clientfd 设置成非阻塞模式，
    // 不能在创建时就设置，这样会影响到 connect 函数的行为
    int old_socket_flag = fcntl(clientfd, F_GETFL, 0);
    int new_socket_flag = old_socket_flag | O_NONBLOCK;
    if (fcntl(clientfd, F_SETFL, new_socket_flag) == -1) {
        close(clientfd);
        std::cout << "set socket to nonblock error." << std::endl;
        return -1;
    }

    // 2.连接服务器
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serveraddr.sin_port = htons(SERVER_PORT);
    for (;;) {
        int ret = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        if (ret == 0) {
            std::cout << "connect to server successfully." << std::endl;
            close(clientfd);
            return 0;
        }
        if (ret == -1) {
            if (errno == EINTR) {
                // connect 动作被信号中断，重试connect
                std::cout << "connecting interruptted by signal, try again." << std::endl;
                continue;
            }
            if (errno == EINPROGRESS) {
                // 连接正在尝试中
                break;
            }
            // 真的出错了，
            close(clientfd);
            return -1;
        }
    }

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(clientfd, &writeset);
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if (select(clientfd + 1, NULL, &writeset, NULL, &tv) != 1) {
        std::cout << "[select] connect to server error." << std::endl;
        close(clientfd);
        return -1;
    }

    int err;
    socklen_t len = static_cast<socklen_t>(sizeof err);
    if (::getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
        close(clientfd);
        return -1;
    }

    if (err == 0) {
        std::cout << "connect to server successfully." << std::endl;
    } else {
        std::cout << "connect to server error." << std::endl;
    }

    // 5. 关闭socket
    close(clientfd);

    return 0;
}