#include "socket_opt.h"
#include "log.h"
#include <asm-generic/socket.h>
#include <cerrno> // errno
#include <cstddef>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h> // readv
#include <unistd.h>  // read

using namespace EasyNet;

/*
关于read和write接口的说明
1.send函数本质上并不是往网络上发送数据,而是将应用层发送缓冲区的数据拷贝到内核缓冲区,
至于什么时候数据会从网卡缓冲区中真正地发到网络中去要根据 TCP/IP 协议栈的行为来确定,
这种行为涉及到一个叫 nagel 算法和 TCP_NODELAY 的 socket 选项。
2.recv函数本质上也并不是从网络上收取数据,而只是将内核缓冲区中的数据拷贝到应用程序的缓冲区中,
当然拷贝完成以后会将内核缓冲区中该部分数据移除。

-----------------------------------------
应用程序A                 应用程序B
[应用缓冲buf]             [应用缓冲buf]
    ↕                         ↕
[soceket内核缓冲区]   ↔   [soceket内核缓冲区]
-----------------------------------------

先上两个重要的结论：
1.
recv/read总是在socket接收缓冲区有数据时立即返回，
只有当receive buffer为空时，blocking模式才会等待，
而nonblock模式下会立即返回-1（errno = EAGAIN或EWOULDBLOCK）

2.
blocking的send/write只有在缓冲区足以放下整个buffer时才返回（与blocking read并不相同）
nonblock write则是返回能够放下的字节数，如果缓冲区满了之后调用则返回-1（errno = EAGAIN或EWOULDBLOCK）
 */

size_t SocketOpt::ReadFdToBuffer(Buffer &buf, int fd) {
    char extrabuf[65536];
    struct iovec vec[2];
    ssize_t n = 0;
    size_t read_size = buf.GetWriteableSize();

    vec[0].iov_base = buf.GetWriteableAddr();
    vec[0].iov_len = read_size;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    do {
        // 可能被系统调用中断,但是实际并没有调用结束,所以用一层while循环.
        const int iovcnt = (read_size < sizeof extrabuf) ? 2 : 1;
        n = ::readv(fd, vec, iovcnt);
    } while (n == -1 && errno == EINTR);

    if (n < 0) {
        // n=-1, errno= EAGAIN时表示 读缓冲区暂时没数据了,需要用户自己拆包确认数据有没有读全,没读全则继续.
    } else if (static_cast<size_t>(n) <= read_size) {
        buf.AdvanceWriter(n);
    } else {
        // 读取的数量超过buf的容量,利用栈上空间.
        buf.SetWriterAddr(buf.GetBufferSize()); // 可写区域已经先被写满了
        buf.Append(extrabuf, n - read_size);
    }

    return n;
}

size_t SocketOpt::WriteBufferToFd(Buffer &buf, int fd) {
    int n = 0;

    do {
        n = static_cast<int>(::write(fd, buf.GetReadableAddr(), buf.GetReadableSize()));
    } while (n == -1 && errno == EINTR);

    if (n == -1 && errno == EAGAIN || errno == EWOULDBLOCK) {
        // fd的发送缓冲区已满,本次没有写入任何数据,需要再次尝试
        return 0;
    }

    if (n > 0) {
        buf.AdvanceReader(n);
    }
    return n; // 当n=-1,且errno不为EAGAIN和EWOULDBLOCK时,就表示出错了
}

bool SocketOpt::SetFdNonblock(int fd) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        // fixme：add log
        return false;
    }
    int r = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return r >= 0;
}

bool SocketOpt::SetFdCloseOnExec(int fd) {
    int flags = ::fcntl(fd, F_GETFD, 0);
    if (flags < 0) {
        // fixme：add log
        return false;
    }
    flags |= FD_CLOEXEC;
    int r = ::fcntl(fd, F_SETFD, flags);
    return r >= 0;
}

bool SocketOpt::SetReuseAddr(int fd) {
    int optval = 1;
    return static_cast<bool>(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) >= 0);
}

bool SocketOpt::SetReusePort(int fd) {
    int optval = 1;
    return static_cast<bool>(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) >= 0);
}

struct sockaddr_in6 SocketOpt::GetLocalAddr(int sockfd) {
    struct sockaddr_in6 localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, (sockaddr *)(&localaddr), &addrlen) < 0) {
        LOG_FATAL("getsockname fial");
    }
    return localaddr;
}

struct sockaddr_in6 SocketOpt::GetPeerAddr(int sockfd) {
    struct sockaddr_in6 peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, (sockaddr *)(&peeraddr), &addrlen) < 0) {
        LOG_FATAL("getpeername fial");
    }
    return peeraddr;
}

int SocketOpt::CreateNonBlockSocket(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_FATAL("createSocket fail, errno=%d", errno);
    }
    return sockfd;
}

int SocketOpt::Connect(int sockfd, const struct sockaddr *addr) {
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

int SocketOpt::Accept(int fd, InetAddress &perrAddr) {
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    socklen_t addrlen = sizeof(struct sockaddr_in6);

    int connfd = ::accept(fd, (struct sockaddr *)&addr, &addrlen);
    if (connfd < 0) {
        LOG_FATAL("accept error=%d", errno);
    }
    perrAddr.setSockAddrInet6(addr);
    SetFdNonblock(connfd);
    SetFdCloseOnExec(connfd);
    return connfd;
}

void SocketOpt::Close(int sockfd) {
    if (::close(sockfd) < 0) {
        LOG_ERROR("close fd=%d fail", sockfd);
    }
}

int SocketOpt::GetSocketError(int sockfd) {
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}

bool SocketOpt::IsSelfConnect(int sockfd) {
    struct sockaddr_in6 localaddr = GetLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = GetPeerAddr(sockfd);
    if (localaddr.sin6_family == AF_INET) {
        const struct sockaddr_in *laddr4 = reinterpret_cast<struct sockaddr_in *>(&localaddr);
        const struct sockaddr_in *raddr4 = reinterpret_cast<struct sockaddr_in *>(&peeraddr);
        return (laddr4->sin_port == raddr4->sin_port) && (laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr);
    } else if (localaddr.sin6_family == AF_INET6) {
        return (localaddr.sin6_port == peeraddr.sin6_port) && (memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr)) == 0;
    } else {
        return false;
    }
}

void SocketOpt::ShutDownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LOG_ERROR("shutdownWrite error");
    }
}
