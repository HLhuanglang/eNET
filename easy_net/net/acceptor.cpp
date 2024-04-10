#include "acceptor.h"
#include "socket_opt.h"
#include "tcp_server.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace EasyNet;

void Acceptor::ProcessReadEvent() {
    InetAddress peerAddr;
    int acceptfd = SocketOpt::Accept(m_fd, peerAddr);
    LOG_TRACE("acceptfd={}", acceptfd);
    if (acceptfd < 0) {
        if (errno == EMFILE) {
            m_idle->ReAccept(m_fd);
        }
    } else {
        // 通知所属的TcpServer有新的连接到来
        m_server->NewConn(acceptfd, peerAddr);
    }
}

void Acceptor::StartListen() {
    // SOMAXCONN定义了系统中每一个端口最大的监听队列的长度
    // cat /proc/sys/net/core/somaxconn 也可以查看
    int ret = SocketOpt::Listen(m_fd, SOMAXCONN);
    if (ret < 0) {
        LOG_ERROR("listen error!");
    }

    EnableRead();
}