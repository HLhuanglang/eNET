#include <sys/epoll.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

#include "notify.h"

void do_some_work() {
    std::cout << time(nullptr) << " :hello,world!" << std::endl;
}

int main() {
    notifier notify;

    std::thread t([&]() {
        sleep(17);
        notify.wakeup();
    });
    t.detach();

    int epfd = ::epoll_create1(0);

    // 将notifier注册到epoll中
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = notify.get_reciver();
    epoll_ctl(epfd, EPOLL_CTL_ADD, notify.get_reciver(), &ev);

    struct epoll_event ee[1024];
    while (true) {
        int n = epoll_wait(epfd, ee, 1024, 5 * 1000);
        if (n < 0 && errno == EINTR) {
            continue;
        }
        for (int i = 0; i < n; i++) {
            if (ee[i].events & (EPOLLHUP | EPOLLERR)) {
                // todo
            }
            if (ee[i].events & EPOLLIN) {
                if (ee[i].data.fd == notify.get_reciver()) {
                    notify.handle_read();
                }
            }
        }
        do_some_work();
    }

    return 0;
}