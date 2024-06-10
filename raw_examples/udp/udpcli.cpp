/*
 * 编译：  g++ udp_client.cc  -o udp_client
 * 运行： ./udp_client port  字符串
 *
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
using namespace std;

#define exit_if(r, ...)                                                                          \
    if (r) {                                                                                     \
        printf(__VA_ARGS__);                                                                     \
        printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
        exit(1);                                                                                 \
    }
std::string serverIp = "127.0.0.1";
int serverPort = 8080;

int main(int argc, const char *argv[]) {
    if (argc < 3) {
        printf("usage:  ./udp_client client_port  sendSomething");
        return 0;
    }
    unsigned short local_port = atoi(argv[1]);
    // udp
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    exit_if(fd < 0, "socket failed");
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof local_addr);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(local_port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    int r = ::bind(fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr));
    exit_if(r, "bind to 0.0.0.0:%d failed %d %s", local_port, errno, strerror(errno));

    struct sockaddr_in other_addr;
    memset(&other_addr, 0, sizeof other_addr);
    other_addr.sin_family = AF_INET;
    other_addr.sin_port = htons(serverPort);
    other_addr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    int i = 2;
    while (1) {
        int r = sendto(fd, argv[2], sizeof(argv[2]), 0, (struct sockaddr *)&other_addr, sizeof(other_addr));
        exit_if(r < 0, "sendto error");

        struct sockaddr_in other;
        socklen_t rsz = sizeof(other);
        char recv_buf[1024];
        int recv_sive = recvfrom(fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&other, &rsz);
        exit_if(recv_sive < 0, "recvfrom error");
        printf("read %d bytes  context is : %s\n", recv_sive, recv_buf);
        sleep(5);
    }
    close(fd);
    return 0;
}