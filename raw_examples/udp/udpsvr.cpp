/*
 * 编译：g++ -std=c++11 udp_server.cc -o udp_server -lpthread
 * 运行： ./udp_server
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

#include <thread>
#include <vector>

using namespace std;

// 默认最多创建5个线程,一个线程对应一个连接
std::vector<std::thread> vthreads;
const int max_threads = 5;

void handlerNewConnect(struct sockaddr_in client_addr);

bool output_log = true;
unsigned short port = 8080;

#define exit_if(r, ...)                                                                          \
    if (r) {                                                                                     \
        printf(__VA_ARGS__);                                                                     \
        printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
        exit(1);                                                                                 \
    }

void setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    exit_if(flags < 0, "fcntl failed");
    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    exit_if(r < 0, "fcntl failed");
}
void setFdpro(int fd, int property) {
    int sockopt = 1;
    if (setsockopt(fd, SOL_SOCKET, property, (void *)&sockopt, sizeof(sockopt)) < 0) {
        exit_if(true, "fd = %d failed  setFdPro", fd);
    }
}

void updateEvents(int efd, int fd, int events, int op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.fd = fd;
    printf("%s fd %d events read %d write %d  to  epoll_fd %d\n", op == EPOLL_CTL_MOD ? "mod" : "add", fd, ev.events & EPOLLIN, ev.events & EPOLLOUT, efd);
    int r = epoll_ctl(efd, op, fd, &ev);
    exit_if(r, "epoll_ctl failed");
}

// 将新用户的地址投递到空闲的线程中。让空闲的线程主动去连接
void handleAccept(int efd, int fd) {
    struct sockaddr_in client_addr;
    socklen_t rsz = sizeof(client_addr);
    size_t size = 1024;
    char recvbuf[size];
    int recv_sive = recvfrom(fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&client_addr, &rsz);
    exit_if(recv_sive < 0, "recvfrom error, %s:%d,  error: %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), strerror(errno));
    if (output_log) {
        printf("man thread recvfrom %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
    if (vthreads.size() < max_threads) {
        // 创建新的线程处理
        std::thread t(handlerNewConnect, client_addr);
        vthreads.push_back(std::move(t));
    } else {
        // 当线程数超过限制   直接忽略该消息。
        printf("because of reaching the limit number of thread , rejust from %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
}

void handleRead(int efd, int fd) {
    // 将收到的内容回传回去
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    size_t size = 1024;
    char recvbuf[size];
    int recv_sive = recvfrom(fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&raddr, &rsz);
    exit_if(recv_sive < 0, "recvfrom error from %s:%d, reason: %s", inet_ntoa(raddr.sin_addr), ntohs(raddr.sin_port), strerror(errno));
    if (output_log) {
        printf("read %d bytes\n", recv_sive);
        printf(" content is : %s\n", recvbuf);
    }
    int r = 0;
    while (recv_sive > 0) {
        r = sendto(fd, recvbuf + r, recv_sive, 0, (struct sockaddr *)&raddr, rsz);
        exit_if(r < 0, "sendto error to  %s:%d, reason: %s", inet_ntoa(raddr.sin_addr), ntohs(raddr.sin_port), strerror(errno));
        recv_sive -= r;
    }
}

void handleWrite(int efd, int fd) {
    // 处理写事件，  暂时留着
}

void loop_once_main(int efd, int lfd, int waitms) {
    const int kMaxEvents = 20;
    struct epoll_event activeEvs[100];
    int n = epoll_wait(efd, activeEvs, kMaxEvents, waitms);
    if (output_log)
        printf("loop_once epoll_wait return %d\n", n);
    for (int i = 0; i < n; i++) {
        int fd = activeEvs[i].data.fd;
        int events = activeEvs[i].events;
        if (events & (EPOLLIN | EPOLLERR)) {
            printf(" fd = %d , read is ok\n", fd);
            if (fd == lfd) {
                handleAccept(efd, fd);
            } else {
                printf("fd is not listen_fd");
                handleRead(efd, fd);
            }
        } else if (events & EPOLLOUT) {
            if (output_log)
                printf("handling epollout\n");
            handleWrite(efd, fd);
        } else {
            exit_if(1, "unknown event");
        }
    }
}
void loop_once_child(int efd, int lfd, int waitms) {
    const int kMaxEvents = 20;
    struct epoll_event activeEvs[100];
    int n = epoll_wait(efd, activeEvs, kMaxEvents, waitms);
    if (output_log)
        printf("loop_once epoll_wait return %d\n", n);
    for (int i = 0; i < n; i++) {
        int fd = activeEvs[i].data.fd;
        int events = activeEvs[i].events;
        if (events & (EPOLLIN | EPOLLERR)) {
            printf("loop_once_child   fd = %d , read is ok\n", fd);
            handleRead(efd, fd);
        } else if (events & EPOLLOUT) {
            if (output_log)
                printf("handling epollout\n");
            handleWrite(efd, fd);
        } else {
            exit_if(1, "unknown event");
        }
    }
}

void handlerNewConnect(struct sockaddr_in client_addr) {
    // 创建一个新的fd,connect对方。raddr
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    setFdpro(client_fd, SO_REUSEADDR);
    setFdpro(client_fd, SO_REUSEPORT);
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof local_addr);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    int r = ::bind(client_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr));
    exit_if(r, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));
    r = connect(client_fd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
    exit_if(r, "connect %s,%d failed: %d,%s", inet_ntoa(client_addr.sin_addr), port, errno, strerror(errno));

    printf(" create fd: %d to %s:%d\n", client_fd, inet_ntoa(client_addr.sin_addr), port);
    // 连接成功之后，发送消息
    char send_buf[1024] = "I am server";
    r = sendto(client_fd, send_buf, sizeof(send_buf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    // r=send(client_fd,send_buf,sizeof(send_buf),0);

    exit_if(r < 0, "sendto %s,%d failed: %d,%s", inet_ntoa(client_addr.sin_addr), port, errno, strerror(errno));
    printf(" sendto %s -> %s:%d \n", send_buf, inet_ntoa(client_addr.sin_addr), port);

    // 为该用户单独创建epoll
    int epollfd = epoll_create(1);
    exit_if(epollfd < 0, "epoll_create failed");

    // 将新的描述符号设置非阻塞
    setNonBlock(client_fd);
    // 将新的客户端fd加入到epoll中监听读事件
    updateEvents(epollfd, client_fd, EPOLLIN, EPOLL_CTL_ADD);
    for (;;) {  // 子线程的轮寻操作
        loop_once_child(epollfd, client_fd, 10000);
    }
}

int main(int argc, const char *argv[]) {
    if (argc > 1) {
        output_log = false;
    }
    int epollfd = epoll_create(1);
    exit_if(epollfd < 0, "epoll_create failed");
    // udp
    int listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    exit_if(listenfd < 0, "socket failed");
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int r = ::bind(listenfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    exit_if(r, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));
    printf(" bind port = %d \n", port);

    setFdpro(listenfd, SO_REUSEADDR);
    setFdpro(listenfd, SO_REUSEPORT);
    setNonBlock(listenfd);
    updateEvents(epollfd, listenfd, EPOLLIN, EPOLL_CTL_ADD);
    for (;;) {
        loop_once_main(epollfd, listenfd, 10000);
    }
    // 退出回收线程资源
    for (auto it = vthreads.begin(); it != vthreads.end(); ++it) {
        it->join();
    }
    return 0;
}