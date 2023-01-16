/*
 * 编译：g++ -o epoll epoll.cc
 * 运行： ./epoll
 * 测试：curl -v localhost:8888
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <map>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

bool output_log = true;

#define exit_if(r, ...)                                                                          \
    if (r) {                                                                                     \
        printf(__VA_ARGS__);                                                                     \
        printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
        exit(1);                                                                                 \
    }

void set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    exit_if(flags < 0, "fcntl failed");
    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    exit_if(r < 0, "fcntl failed");
}

void update_events(int efd, int fd, int events, int op)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events  = events;
    ev.data.fd = fd;
    printf("%s fd %d events read %d write %d\n", op == EPOLL_CTL_MOD ? "mod" : "add", fd, ev.events & EPOLLIN, ev.events & EPOLLOUT);
    int r = epoll_ctl(efd, op, fd, &ev);
    exit_if(r, "epoll_ctl failed");
}

void handle_accept(int efd, int fd)
{
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd       = accept(fd, (struct sockaddr *)&raddr, &rsz);
    exit_if(cfd < 0, "accept failed");
    struct sockaddr_in peer, local;
    socklen_t alen = sizeof(peer);
    int r          = getpeername(cfd, (struct sockaddr *)&peer, &alen);
    exit_if(r < 0, "getpeername failed");
    printf("accept a connection from %s\n", inet_ntoa(raddr.sin_addr));
    set_nonblock(cfd);
    update_events(efd, cfd, EPOLLIN, EPOLL_CTL_ADD);
}

void handle_read(int efd, int fd)
{
    int total_size;
    int ret = 0;
    if (::ioctl(fd, FIONREAD, &total_size) == -1) {
        printf("ioctl FIONREAD\n");
    }
    printf("totoal_size=%d\n", total_size);
    char buf[total_size];
    do {
        ret = ::read(fd, buf, total_size);
    } while (ret == -1 && errno == EINTR);
    printf("%s", buf);
    if (ret == 0) {
        update_events(efd, fd, EPOLLIN, EPOLL_CTL_DEL);
    } else {
        const char *str = "hello,world!";
        write(fd, str, strlen(str));
    }
}

void handle_write(int efd, int fd)
{
    // todo
    printf("write todo\n");
}

void loop_once(int efd, int lfd, int waitms)
{
    const int kMaxEvents = 20;
    struct epoll_event activeEvs[100];
    int n = epoll_wait(efd, activeEvs, kMaxEvents, waitms);
    if (output_log) printf("epoll_wait return %d\n", n);
    for (int i = 0; i < n; i++) {
        int fd     = activeEvs[i].data.fd;
        int events = activeEvs[i].events;
        if (events & (EPOLLIN | EPOLLHUP | EPOLLERR)) {
            if (fd == lfd) {
                handle_accept(efd, fd);
            } else {
                handle_read(efd, fd);
            }
        } else if (events & (EPOLLOUT | EPOLLHUP | EPOLLERR)) {
            if (output_log) printf("handling epollout\n");
            handle_write(efd, fd);
        } else {
            exit_if(1, "unknown event");
        }
    }
}

int main(int argc, const char *argv[])
{
    if (argc > 1) {
        output_log = false;
    }
    ::signal(SIGPIPE, SIG_IGN);
    unsigned short port = 8888;
    int epollfd         = epoll_create1(0);
    exit_if(epollfd < 0, "epoll_create failed");
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    exit_if(listenfd < 0, "socket failed");
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int r                = ::bind(listenfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    exit_if(r, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));
    r = listen(listenfd, SOMAXCONN);
    exit_if(r, "listen failed %d %s", errno, strerror(errno));
    printf("fd %d listening at %d\n", listenfd, port);
    set_nonblock(listenfd);
    update_events(epollfd, listenfd, EPOLLIN, EPOLL_CTL_ADD);
    for (;;) { // 实际应用应当注册信号处理函数，退出时清理资源
        loop_once(epollfd, listenfd, 10000);
    }
    return 0;
}
