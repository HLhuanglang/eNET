#ifndef __EASYNET_ACCEPTOR_H
#define __EASYNET_ACCEPTOR_H

#include <asm-generic/errno-base.h>
#include <cerrno>
#include <fcntl.h>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>

#include "fd_event.h"
#include "log.h"
#include "tcp_server.h"

class idle_fd {
 public:
    idle_fd() {
        m_idlefd = ::open("/tmp/easy_net_idle", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
        if (m_idlefd < 0) {
            LOG_FATAL("create idlefd failed!");
        }
    }

    ~idle_fd() {
        ::close(m_idlefd);
    }

    void use(int fd) {
        std::lock_guard<std::mutex> lg(m_mtx);
        // 1,释放fd用于处理新的链接
        ::close(m_idlefd);
        // 2,处理链接
        m_idlefd = ::accept(fd, nullptr, nullptr);
        // 3,处理完毕,重新占位
        ::close(m_idlefd);
        m_idlefd = ::open("/tmp/easy_net_idlefd", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
        if (m_idlefd < 0) {
            // LOGO_FATAL("create idlefd failed!");
        }
    }

 private:
    std::mutex m_mtx;
    int m_idlefd;
};

class acceptor : public fd_event {
 public:
    // fd为listenfd
    acceptor(tcp_server *svr, event_loop *loop, int fd) : fd_event(loop, fd), m_server(svr) {
        m_idle = new idle_fd();
    }

 public:
    void handle_read() override;

 private:
    idle_fd *m_idle;
    tcp_server *m_server;
};

#endif