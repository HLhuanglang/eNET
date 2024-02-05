#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#define SOCKET_NAME "/tmp/my_socket"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;

    // 创建域套接字
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 连接到服务端的套接字
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // 发送消息给服务端
    const char *msg = "Hello, server!";
    ssize_t n = write(client_fd, msg, strlen(msg));
    if (n == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // 关闭套接字
    close(client_fd);

    return 0;
}