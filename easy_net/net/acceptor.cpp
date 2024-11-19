#include "acceptor.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cerrno>

#include "socket_opt.h"

using namespace EasyNet;

void Acceptor::ProcessReadEvent() {
    InetAddress peerAddr;
    // accept是从已经完成三次握手的tcp队列中取出一个连接的。
    // 假设在accept之前，客户端主动发送RST终止连接，如果监听套接字是阻塞的，那么服务器就会一直阻塞在accept调用上，直到新的连接到来accept才会返回。
    // 所以一般socket()创建监听套接字后，都会设置成非阻塞的。
    // 当客户在服务器调用 accept 之前中止某个连接时，accept 调用可以立即返回 -1
    // 这时源自 Berkeley 的实现会在内核中处理该事件，并不会将该事件通知给 epoll
    // 而其他实现把 errno 设置为 ECONNABORTED 或者 EPROTO 错误，我们应该忽略这两个错误。
    int acceptfd = SocketOpt::Accept(m_fd, peerAddr);
    LOG_TRACE("acceptfd={}", acceptfd);
    if (acceptfd < 0) {
        if (errno == EMFILE) {
            m_idle->ReAccept(m_fd);
        }
    } else {
        // 通知所属的TcpServer有新的连接到来
        // 因为acceptor是属于tcpserver的，由内核决定唤醒哪个线程来处理新的连接
        // 因此,新建立的链接一定是和某个tcpserver关联的.
        m_server->NewConn(acceptfd, peerAddr);
    }
}

void Acceptor::StartListen() {
    // SOMAXCONN定义了系统中每一个端口最大的监听队列的长度
    // cat /proc/sys/net/core/somaxconn 也可以查看
    // https://www.cnxct.com/something-about-phpfpm-s-backlog/
    int ret = SocketOpt::Listen(m_fd, SOMAXCONN);
    if (ret < 0) {
        LOG_ERROR("listen error!");
    }

    EnableRead();
}
