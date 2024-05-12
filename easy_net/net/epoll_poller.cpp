#include "epoll_poller.h"

#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "io_event.h"
#include "log.h"

using namespace EasyNet;

// epoll、poll使用一样的值
///
/// POLLIN     0x0001 普通或优先级带数据可读
/// POLLRDNORM 0x0040 普通数据可读
/// POLLRDBAND 0x0080 优先级带数据可读
/// POLLPRI    0x0002 高优先级数据可读
///
/// POLLOUT    0x0004 可以不阻塞的写普通数据和优先级数据
/// POLLWRNORM 0x0100 可以不阻塞的写普通数据
/// POLLWRBAND 0x0200 可以不阻塞的写优先级带数据
///
/// POLLERR    0x0008 发生错误
/// POLLHUP    0x0010 管道的写端被关闭，读端描述符上接收到这个事件(对端调用shoutdown(SHUT_WR)表示不再写入数据,此时本端感知到)
/// POLLNVAL   0x0020 描述符不是一个打开的文件
///
/// POLLRDHUP  0x2000 TCP连接被对方关闭，或者对方关闭了写操作
///            客户端调用close()正常断开连接，在服务器端会触发一个
///            事件。在低于 2.6.17 版本的内核中，这个事件EPOLLIN即0x1
///            代表连接可读。然后服务器上层软件read连接，只能读到 EOF
///            2.6.17 以后的版本增加了EPOLLRDHUP事件，对端连接断开触发
///            的事件会包含 EPOLLIN | EPOLLRDHUP，即 0x2001
///
static_assert(EPOLLIN == POLLIN, "EPOLLIN");
static_assert(EPOLLPRI == POLLPRI, "EPOLLPRI");
static_assert(EPOLLOUT == POLLOUT, "EPOLLOUT");
static_assert(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP");
static_assert(EPOLLERR == POLLERR, "EPOLLERR");
static_assert(EPOLLHUP == POLLHUP, "EPOLLHUPEPOLLHUP");

EpollPoller::EpollPoller(EventLoop *loop) : Poller(loop) {
    m_epollfd = ::epoll_create1(0);
    if (m_epollfd < 0) {
        LOG_ERROR("epoll_create1 fail");
    }
    LOG_TRACE("epollfd={}", m_epollfd);
}

EpollPoller::~EpollPoller() {
    ::close(m_epollfd);
}

// typedef union epoll_data
// {
//     void *ptr;
//     int fd;
//     uint32_t u32;
//     uint64_t u64;
// }
// epoll_data_t;

// struct epoll_event {
//     uint32_t events;   /* Epoll events */
//     epoll_data_t data; /* User data variable */
// }

void EpollPoller::Polling(int timeout_ms, active_events_t &events) {
    // 1,获取激活的IO事件
    struct epoll_event epoll_events[1024];
    // timeout -1：阻塞, 会一直等待IO事件到达，如果没有就一直阻塞在这里。
    // timeout 0：不阻塞，立即返回，如果没有IO事件到达，就返回0
    int nfds = ::epoll_wait(m_epollfd, epoll_events, 1024, timeout_ms);
    if (nfds < 0) {
        LOG_ERROR("epoll_wait fail");
    }

    // 2,处理IO事件
    for (int i = 0; i < nfds; ++i) {
        int mask = 0;
        struct epoll_event e = epoll_events[i];
        auto *ev = static_cast<IOEvent *>(e.data.ptr);
        if (e.events & (POLLIN | POLLPRI | POLLRDHUP))
            mask |= IOEvent_READABLE;
        if (e.events & POLLOUT)
            mask |= IOEvent_WRITEABLE;
        if (e.events & (POLLNVAL | POLLERR))
            mask |= IOEvent_WRITEABLE | IOEvent_READABLE;
        if (e.events & POLLHUP)
            mask |= IOEvent_WRITEABLE | IOEvent_READABLE;
        ev->SetFiredEvents(mask);
        events.push_back(ev);
    }
}

void EpollPoller::AddEvent(IOEvent *ev) {
    int fd = ev->GetFD();
    int events = ev->GetExpectEvent();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) < 0) {
        LOG_ERROR("epollfd={} add fd = {} events = {} fail", m_epollfd, fd, events);
    } else {
        LOG_TRACE("epollfd={} add {} events = {}", m_epollfd, fd, events);
    }
}

void EpollPoller::DelEvent(IOEvent *ev) {
    int fd = ev->GetFD();
    int events = ev->GetExpectEvent();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &event) < 0) {
        LOG_ERROR("epollfd={} del fd = {} events = {} fail", m_epollfd, fd, events);
    } else {
        LOG_TRACE("epollfd={} del {} events = {}", m_epollfd, fd, events);
    }
}

void EpollPoller::ModEvent(IOEvent *ev) {
    int fd = ev->GetFD();
    int events = ev->GetExpectEvent();

    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = events;
    event.data.ptr = ev;

    if (::epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &event) < 0) {
        LOG_ERROR("epollfd={} mod fd = {} events = {} fail", m_epollfd, fd, events);
    } else {
        LOG_TRACE("epollfd={} mod {} events = {}", m_epollfd, fd, events);
    }
}