#include <assert.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <vector>

void handle_sig(int) {
    std::cout << "bye~" << std::endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_sig);
    int timer_fd = ::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    struct itimerspec newtimer;
    memset(&newtimer, 0, sizeof(itimerspec));
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    std::cout << "timer begin:" << now.tv_sec << std::endl;
    // 程序运行后第一次超时时间（用这个作为定时器时间即可）
    newtimer.it_value.tv_nsec = 0;
    newtimer.it_value.tv_sec = 5;
    // 之后每次超时间隔（不使用）
    //  newtimer.it_interval.tv_nsec = 0;
    //  newtimer.it_interval.tv_sec = 5;

    int ret = ::timerfd_settime(timer_fd, 0, &newtimer, nullptr);

    int ep_fd = epoll_create(1);  // 创建epoll实例对象
    struct epoll_event ev;
    std::vector<struct epoll_event> ready_events(16);
    ev.data.fd = timer_fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, timer_fd, &ev);  // 添加到epoll事件集合
    for (;;) {
        int nfd = epoll_wait(ep_fd, &*ready_events.begin(), static_cast<int>(ready_events.size()), 0);
        if (nfd > 0) {
            for (int i = 0; i < nfd; i++) {
                if (ready_events[i].events & EPOLLIN) {
                    if (ready_events[i].data.fd == timer_fd) {
                        // 如果不读取数据,之后epoll_wait会阻塞,但是可以采取重新设置的方式来设定超时时间
                        //  uint64_t exp;
                        //  uint64_t s = read(ready_events[i].data.fd, &exp, sizeof(uint64_t));
                        //  assert(s == sizeof(uint64_t));
                        clock_gettime(CLOCK_REALTIME, &now);
                        std::cout << "time:" << now.tv_sec << std::endl;

                        newtimer.it_value.tv_sec = 5;
                        ::timerfd_settime(timer_fd, 0, &newtimer, nullptr);
                    }
                }
            }
        } else if (nfd == 0) {
            // nothing happen
        } else {
            // error happen
        }
    }
    close(timer_fd);
    close(ep_fd);

    return 0;
}