#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

using msg_type_t = std::uint64_t;

void handle_sig(int) {
    std::cout << "bye~" << std::endl;
    exit(0);
}

void thread_func(int fd) {
    int ep_fd = ::epoll_create1(0);
    std::vector<struct epoll_event> ready_events(16);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &ev);
    for (;;) {
        int nfd = ::epoll_wait(ep_fd, &*ready_events.begin(), static_cast<int>(ready_events.size()), 1 * 1000);
        if (nfd > 0) {
            for (int i = 0; i < nfd; i++) {
                if (ready_events[i].events & (EPOLLHUP | EPOLLERR)) {
                    // todo
                }
                if (ready_events[i].events & EPOLLIN) {
                    msg_type_t msg;
                    int ret = ::read(ready_events[i].data.fd, &msg, sizeof(msg_type_t));
                    if (ret == -1) {
                        perror("eventfd write");
                    } else {
                        std::cout << "reciv main thread notify!" << std::endl;
                    }
                }
            }
        } else if (nfd == 0) {
            // nothing happen
        } else {
            // error happen
        }
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handle_sig);
    int event_fd = ::eventfd(0, EFD_NONBLOCK);
    std::thread t(thread_func, event_fd);
    for (int i = 0; i < 5; i++) {
        msg_type_t msg = 1;  // 必须赋值,否则无法触发.
        int ret = ::write(event_fd, &msg, sizeof(msg_type_t));
        if (ret == -1) {
            perror("eventfd write");
        } else {
            std::cout << "send signal to sub thread!" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    t.join();
}