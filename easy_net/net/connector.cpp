#include "connector.h"

#include "def.h"
#include "log.h"
#include "socket_opt.h"

using namespace EasyNet;

// 异步非阻塞connect的流程：
// 1. 创建socket，并将 socket 设置成非阻塞模式；
// 2. 调用 connect 函数，此时无论 connect 函数是否连接成功会立即返回；如果返回-1并不表示连接出错，如果此时错误码是EINPROGRESS表示连接还未完成.
// 3. 接着调用 select 函数，在指定的时间内判断该 socket 是否可写，如果可写说明连接成功，反之则认为连接失败。
void Connector::Start() {
    m_fd = SocketOpt::CreateNonBlockSocket(m_addr.family());
    LOG_DEBUG("CreateNonBlockSocket fd={} addr={}", m_fd, m_addr.SerializationToIpPort());
    int ret = SocketOpt::Connect(m_fd, m_addr.GetAddr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            LOG_DEBUG("Connecting...");
            m_status = ConnectState::CONNECTING;
            EnableWrite();  // 使能写,必定在下一次loop中能唤醒，进行回调
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            LOG_DEBUG("Retry error={} msg={}", savedErrno, strerror(savedErrno));
            Retry();
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_ERROR("connect error={} msg={}", savedErrno, strerror(savedErrno));
            SocketOpt::Close(m_fd);
            break;

        default:
            LOG_ERROR("Unexpected error={} msg={}", savedErrno, strerror(savedErrno));
            SocketOpt::Close(m_fd);
    }
}

void Connector::ProcessWriteEvent() {
    if (m_status == ConnectState::CONNECTING) {
        RemoveEvent();  // 只是为了完成异步connect,此时不再关注写事件,由后续客户端是否发送数据来决定是否关注写事件

        int err = SocketOpt::GetSocketError(m_fd);
        if (err) {
            LOG_ERROR("connect fail={}", strerror(err));
            m_status = ConnectState::CONNERROR;
            Retry();
        } else if (SocketOpt::IsSelfConnect(m_fd)) {
            LOG_ERROR("Self connect");
            m_status = ConnectState::CONNERROR;
            Retry();
        } else {
            m_status = ConnectState::CONNECTED;
            m_client->NewConn(m_fd, m_addr);  // 完成了tcp三次握手,通知上层,可以开始发送数据了
            m_fd = KInvalidFD;
        }
    }
}

void Connector::Retry() {
    if (m_status == ConnectState::CONNECTING) {
        // 关闭fd之前，需要确保fd已经从epoll监控中移除. 避免epoll的幽灵事件问题
        // 参考：https://andypan.me/zh-hans/posts/2024/08/23/linux-epoll-with-level-triggering-and-edge-triggering
        // CONNERROR状态时已移除了监控,可以直接关闭
        RemoveEvent();
    }
    m_status = ConnectState::DISCONNECTED;
    SocketOpt::Close(m_fd);
    m_ioloop->TimerAfter(std::bind(&Connector::ReConnect, this), m_retry_delay_ms);
    m_retry_delay_ms = m_retry_delay_ms * 2;
}

void Connector::ReConnect() {
    m_ioloop->RunInLoop([&]() {
        if (m_status == ConnectState::DISCONNECTED) {
            if (m_retry_delay_ms <= KMaxRetryTimeMS) {
                Start();
            } else {
                LOG_ERROR("Retry too many times, give up!");  // fixme：这里停止尝试后,需要告知上层?
            }
        }
    });
}
