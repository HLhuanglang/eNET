#ifndef __EASYNET_ACCEPTOR_H
#define __EASYNET_ACCEPTOR_H

#include <asm-generic/errno-base.h>
#include <cerrno>
#include <fcntl.h>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>

#include "fd_event.h"

class idle_fd {
public:
    idle_fd() {
        idlefd_ = ::open("/tmp/easy_net_idle", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
        if (idlefd_ < 0) {
            //LOGO_FATAL("create idlefd failed!");
        }
    }

    ~idle_fd() {
        ::close(idlefd_);
    }

    void use(int fd) {
        std::lock_guard<std::mutex> lg(mtx_);
        //1,释放fd用于处理新的链接
        ::close(idlefd_);
        //2,处理链接
        idlefd_ = ::accept(fd, NULL, NULL);
        //3,处理完毕,重新占位
        ::close(idlefd_);
        idlefd_ = ::open("/tmp/easy_net_idlefd", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
        if (idlefd_ < 0) {
            //LOGO_FATAL("create idlefd failed!");
        }
    }

private:
    std::mutex mtx_;
    int idlefd_;
};

class acceptor : public fd_event {
public:
    //fd为listenfd
    acceptor(event_loop *loop, int fd) : fd_event(loop, fd) {
        idle_ = new idle_fd();
    }

private:
    idle_fd *idle_;
};

#endif