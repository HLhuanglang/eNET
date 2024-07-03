#include "epoll_poller.h"

#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <sstream>

#include "io_event.h"
#include "log.h"

using namespace EasyNet;

EpollPoller::EpollPoller(EventLoop *loop) : Poller(loop) {
    m_epollfd = ::epoll_create1(0);
    if (m_epollfd < 0) {
        LOG_ERROR("epoll_create1 fail");
    }
    LOG_TRACE("epollfd={}", m_epollfd);
}

EpollPoller::~EpollPoller() {
    SocketOpt::Close(m_epollfd);
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
        LOG_ERROR("epoll_wait fail :{}", strerror(errno));
    }

    // 2,处理IO事件
    /// EPOLLIN     0x0001 表示有数据可读。当一个socket接收到数据时，该事件被触发。这通常发生在TCP连接的另一端发送数据到该socket时。
    /// EPOLLPRI    0x0002 表示有紧急数据可读。这通常用于带外数据（out-of-band data），这是一种优先级较高的数据传输机制。
    ///             在大多数情况下，TCP不使用带外数据，因此这个事件在普通的网络编程中很少使用。
    /// EPOLLOUT    0x0004 表示socket可写。当一个socket的发送缓冲区有足够的空间来容纳更多的数据时，该事件被触发。
    ///             这通常发生在TCP连接建立后，或者在之前的数据发送完成后。
    /// EPOLLMSG    0x400 表示有消息可读。这通常用于UNIX域套接字（Unix domain sockets）。
    /// EPOLLERR    0x0008 发生错误
    /// EPOLLHUP    0x0010 表示连接挂起。管道的写端被关闭，读端描述符上接收到这个事件(对端调用shoutdown(SHUT_WR)表示不再写入数据,此时本端感知到)
    /// EPOLLRDHUP  0x2000 TCP连接被对方关闭，或者对方关闭了写操作
    ///             客户端调用close()正常断开连接，在服务器端会触发一个
    ///             事件。在低于 2.6.17 版本的内核中，这个事件EPOLLIN即0x1
    ///             代表连接可读。然后服务器上层软件read连接，只能读到 EOF
    ///             2.6.17 以后的版本增加了EPOLLRDHUP事件，对端连接断开触发
    ///             的事件会包含 EPOLLIN | EPOLLRDHUP，即 0x2001
    /// EPOLLEXCLUSIVE：(since linux4.5)表示独占事件。当设置此标志时，epoll仅向一个线程报告事件，即使多个线程在同一个epoll实例上等待事件。这可以用于避免竞争条件和锁争用。
    /// EPOLLET     表示边缘触发（Edge-Triggered）模式。当设置此标志时，epoll仅在事件状态发生变化时报告事件。
    ///             这意味着，如果事件仍然有效，epoll不会再次报告该事件，除非事件状态再次发生变化。这种模式通常用于高性能的网络编程，因为它减少了事件通知的频率，从而提高了效率。
    for (int i = 0; i < nfds; ++i) {
        int mask = 0;
        struct epoll_event e = epoll_events[i];
        auto *ev = static_cast<IOEvent *>(e.data.ptr);
        if (e.events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
            mask |= IOEvent_READABLE;
        if (e.events & EPOLLOUT)
            mask |= IOEvent_WRITEABLE;
        if (e.events & EPOLLERR)
            mask |= IOEvent_WRITEABLE | IOEvent_READABLE;
        if (e.events & EPOLLHUP)
            mask |= IOEvent_WRITEABLE | IOEvent_READABLE;
        ev->SetFiredEvents(mask);
        events.push_back(ev);
        LOG_DEBUG("epollfd={} fd={} events={} mask={}", m_epollfd, ev->GetFD(), GetEventString(e.events), mask);
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
        LOG_ERROR("epollfd={} add fd = {} events = {} fail: {}", m_epollfd, fd, events, strerror(errno));
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
        LOG_ERROR("epollfd={} del fd = {} events = {} fail: {}", m_epollfd, fd, events, strerror(errno));
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
        LOG_ERROR("epollfd={} mod fd = {} events = {} fail: {}", m_epollfd, fd, events, strerror(errno));
    } else {
        LOG_TRACE("epollfd={} mod {} events = {}", m_epollfd, fd, events);
    }
}

std::string EpollPoller::GetEventString(uint32_t events) const {
    std::ostringstream oss;

    if (events & EPOLLIN) {
        oss << "EPOLLIN ";
    }
    if (events & EPOLLPRI) {
        oss << "EPOLLPRI ";
    }
    if (events & EPOLLOUT) {
        oss << "EPOLLOUT ";
    }
    if (events & EPOLLRDNORM) {
        oss << "EPOLLRDNORM ";
    }
    if (events & EPOLLRDBAND) {
        oss << "EPOLLRDBAND ";
    }
    if (events & EPOLLWRNORM) {
        oss << "EPOLLWRNORM ";
    }
    if (events & EPOLLWRBAND) {
        oss << "EPOLLWRBAND ";
    }
    if (events & EPOLLMSG) {
        oss << "EPOLLMSG ";
    }
    if (events & EPOLLERR) {
        oss << "EPOLLERR ";
    }
    if (events & EPOLLHUP) {
        oss << "EPOLLHUP ";
    }
    if (events & EPOLLRDHUP) {
        oss << "EPOLLRDHUP ";
    }
    if (events & EPOLLEXCLUSIVE) {
        oss << "EPOLLEXCLUSIVE ";
    }
    if (events & EPOLLWAKEUP) {
        oss << "EPOLLWAKEUP ";
    }
    if (events & EPOLLONESHOT) {
        oss << "EPOLLONESHOT ";
    }
    if (events & EPOLLET) {
        oss << "EPOLLET ";
    }

    std::string result = oss.str();
    if (!result.empty()) {
        // Remove the trailing space
        result.pop_back();
    }
    return result;
}