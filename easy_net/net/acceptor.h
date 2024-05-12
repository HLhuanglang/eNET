#ifndef __EASYNET_ACCEPTOR_H
#define __EASYNET_ACCEPTOR_H

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <memory>

#include "io_event.h"
#include "log.h"
#include "non_copyable.h"
#include "socket_opt.h"
#include "tcp_server.h"

namespace EasyNet {
class IdleFD {
 public:
    IdleFD() {
        m_idlefd = ::open("/tmp/easy_net_idle", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
        if (m_idlefd < 0) {
            LOG_ERROR("create idlefd failed!");
        }
        LOG_TRACE("idlefd={}", m_idlefd);
    }

    ~IdleFD() {
        ::close(m_idlefd);
    }

    void ReAccept(int fd) {
        // 1,释放fd用于处理新的链接
        ::close(m_idlefd);
        // 2,处理链接
        m_idlefd = ::accept(fd, nullptr, nullptr);
        // 3,处理完毕,重新占位
        ::close(m_idlefd);
        m_idlefd = ::open("/tmp/easy_net_idlefd", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
        if (m_idlefd < 0) {
            LOG_ERROR("create idlefd failed!");
        }
        LOG_WARN("ReAccept new idlefd={}", m_idlefd);
    }

 private:
    int m_idlefd;
};

class Acceptor : public IOEvent {
 public:
    // fd为listenfd
    Acceptor(TcpServer *svr, const InetAddress &listenAddr, bool isReusePort)
        : IOEvent(svr->GetEventLoop(), SocketOpt::CreateNonBlockSocket(listenAddr.family())),
          m_server(svr) {
        m_idle = make_unique<IdleFD>();

        // 1,socket：初始化时候已经创建完成
        SocketOpt::SetReuseAddr(m_fd);
        if (isReusePort) {
            SocketOpt::SetReusePort(m_fd);
        }

        // 2,bind
        int ret = ::bind(m_fd, listenAddr.GetAddr(), listenAddr.GetAddrSize());
        if (ret < 0) {
            LOG_ERROR("bindAddress error!");
        }

        // 3,StartListen，由tcp_server来控制开启时机
    }

 public:
    void StartListen();

 public:
    /// @brief 处理新连接
    void ProcessReadEvent() override;

 private:
    std::unique_ptr<IdleFD> m_idle;
    TcpServer *m_server;  // 当前acceptor属于哪一个TcpServer,生命周期由TcpServer控制
};
}  // namespace EasyNet

#endif