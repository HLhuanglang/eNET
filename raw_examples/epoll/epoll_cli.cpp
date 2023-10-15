/*
    关注epoll et和lt模式下，write的现象
*/
#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_INPUT_CHAR 256
#define MAX_READ 256
#define WRITE_NUM 8

const int k_port = 8888;

struct WriteBuffer {
    struct WriteBuffer *next;
    int size;
    char *ptr;
    char *buf;
};

struct WbList {
    struct WriteBuffer *head;
    struct WriteBuffer *tail;
};

static struct WbList wb_list;

static void *new_buffer(int size) {
    auto *wb = (struct WriteBuffer *)malloc(sizeof(struct WriteBuffer));
    wb->size = size;
    wb->buf = (char *)malloc(sizeof(char) * size);
    wb->ptr = wb->buf;
    wb->next = nullptr;
}

static void free_buffer(struct WriteBuffer *wb) {
    free(wb->buf);
    free(wb);
}

static int try_connect(const char *host, int port) {
    struct addrinfo ai_hints;
    struct addrinfo *ai_list = NULL;

    char portstr[16];
    sprintf(portstr, "%d", port);
    memset(&ai_hints, 0, sizeof(ai_hints));

    ai_hints.ai_family = AF_INET;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(host, portstr, &ai_hints, &ai_list);
    if (status != 0) {
        freeaddrinfo(ai_list);
        printf("getaddrinfo fail");
        return -1;
    }

    int fd = socket(ai_list->ai_family, ai_list->ai_socktype, ai_list->ai_protocol);
    if (fd < 0) {
        freeaddrinfo(ai_list);
        return -1;
    }
    status = connect(fd, ai_list->ai_addr, ai_list->ai_addrlen);
    if (status != 0) {
        freeaddrinfo(ai_list);
        close(fd);
        printf("connect fail");
        return -1;
    }

    freeaddrinfo(ai_list);
    printf("connect to server success \n");
    return fd;
}

static void input(int epfd, int fd, int event) {
    printf("begin input\n");
    char buf[MAX_INPUT_CHAR];
    int idx = 0;
    for (;;) {
        int c = getchar();
        if (c == '\n' || idx >= MAX_INPUT_CHAR - 1) {
            break;
        }

        // reset epollout
        if (c == '~') {
            struct epoll_event e;
            e.events = event & (~EPOLLET);
            e.data.fd = fd;
            epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &e);

            e.events = event | EPOLLOUT;
            epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &e);
            printf("epollout reset\n");
            continue;
        }

        buf[idx] = c;
        idx++;
    }
    buf[idx] = '\0';

    if (idx > 0) {
        struct WriteBuffer *wb = (WriteBuffer *)(new_buffer(idx));
        if (wb_list.tail == NULL) {
            wb_list.head = wb_list.tail = wb;

            struct epoll_event ee;
            ee.events = event;
            ee.data.fd = fd;
            epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ee);
        } else {
            if (wb_list.head == wb_list.tail) {
                wb_list.head->next = wb;
            }
            wb_list.tail->next = wb;
            wb_list.tail = wb;
        }
        memcpy(wb->buf, buf, sizeof(char) * idx);
    }
    printf("%s\n", buf);
    printf("end input\n");
}

static void output(int epfd, int fd, int event) {
    printf("begin to output\n");
    struct WriteBuffer *wb = wb_list.head;
    if (wb) {
        int wsize = wb->size - (wb->ptr - wb->buf);
        if (wsize > WRITE_NUM) {
            wsize = WRITE_NUM;
        }
        int n = write(fd, wb->ptr, wsize);
        if (n <= 0) {
            abort();
        }

        printf("wsize:%d n:%d\n", wsize, n);
        printf("<<<<");
        char *ptr = (char *)wb->ptr;
        for (int i = 0; i < n; i++) {
            printf("%c", ptr[i]);
        }
        printf("\n");

        wb->ptr += wsize;
        if (wb->ptr >= wb->buf + wb->size) {
            if (wb_list.head == wb_list.tail) {
                wb_list.head = wb_list.tail = NULL;
                free_buffer(wb);

                struct epoll_event ee;
                ee.events = event;
                ee.data.fd = fd;
                epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ee);
            } else {
                wb_list.head = wb_list.head->next;
                free_buffer(wb);
            }
        }
    }
    printf("end to output\n");
}

int main(int argc, char **argv) {
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

    int epfd = epoll_create1(0);
    if (epfd < 0) {
        std::cerr << "Failed to create epoll" << std::endl;
        exit(EXIT_FAILURE);
    }

    int fd = try_connect("127.0.0.1", 8888);
    if (fd < 0) {
        return -1;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK);

    struct epoll_event e;
    e.events = EPOLLIN | epoll_type;
    e.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &e);

    wb_list.head = wb_list.tail = nullptr;

    for (;;) {
        input(epfd, fd, EPOLLIN | EPOLLOUT | epoll_type);

        printf("before epoll_wait\n");
        std::array<epoll_event, 1> ev;
        int n = epoll_wait(epfd, ev.begin(), 1, -1);
        if (n == -1) {
            printf("epoll_wait error %d", errno);
            break;
        }
        printf("after epoll_wait\n");

        for (int i = 0; i < n; i++) {
            struct epoll_event *e = &ev[i];
            int flag = e->events;
            int r = (flag & EPOLLIN) != 0;
            int w = (flag & EPOLLOUT) != 0;

            if (r) {
                printf("begin to read fd:%d\n", fd);
                char read_buf[MAX_READ] = {0};
                int rn = read(fd, read_buf, MAX_READ);
                if (rn <= 0) {
                    goto _EXIT;
                }

                printf(">>>>");
                for (int j = 0; j < rn; j++) {
                    printf("%c", read_buf[j]);
                }
                printf("\n");
            }

            if (w) {
                output(epfd, fd, EPOLLIN | epoll_type);
            }
        }
    }

_EXIT:
    close(fd);
    close(epfd);

    return 0;
}