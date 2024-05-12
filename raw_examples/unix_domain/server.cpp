#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#define SOCKET_NAME "/tmp/my_socket"

int main() {
    int server_fd;
    int client_fd;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    socklen_t client_len;

    // 创建域套接字
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_NAME);  // bind时会创建文件,如果不提前删除了就会导致bind失败...
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听套接字
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 接受客户端连接
    client_len = sizeof(struct sockaddr_un);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 读取客户端发送的数据
    char buf[1024];
    ssize_t n = read(client_fd, buf, sizeof(buf));
    if (n == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Received message: %s\n", buf);

    // 关闭套接字
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_NAME);

    return 0;
}