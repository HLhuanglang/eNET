#include "tcp_connection.h"

#include <fcntl.h>

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

#include "buffer.h"
#include "log.h"
#include "socket_opt.h"

using namespace EasyNet;

TcpConn::TcpConn(ConnOwner *owner, int fd, const InetAddress &perrAddr) : IOEvent(owner->GetEventLoop(), fd), m_owner(owner) {
    // 1,初始化buffer
    m_read_buf = new Buffer();
    m_write_buf = new Buffer();

    // 2,初始化链接名称
    auto ip_port = perrAddr.SerializationToIpPort();
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    m_name = std::to_string(timestamp) + "_" + ip_port;

    // 3,设置状态
    // 只能保证TCP连接是正常的,当对端掉电或者网络断开时,通过tcp保活机制来检测连接是否正常,不能确保业务是否存活。
    // 业务是否存活要依赖于心跳机制,心跳机制是业务层面的,不是网络层面的。
    SocketOpt::SetKeepAlive(fd, true);
    m_status = ConnStatus::CONNECTING;
}

TcpConn::~TcpConn() {
    // 避免释放连接时候内存泄漏
    if (m_read_buf) {
        delete m_read_buf;
    }
    if (m_write_buf) {
        delete m_write_buf;
    }
}

void TcpConn::KeepAlive() {
    // TODO
}

void TcpConn::SendData(const char *data, size_t data_size) {
    // 这个函数主要是为了做异步发送，上层调用直接往buf里面写，不让上层阻塞在write上面
    // 上层调用的,只要判断buf中有数据就将EPOLLOUT事件添加到epoll,传入回调。在回调中调用write_buf_to_fd
    //(上层调用有个小优化逻辑,就是数据必须达到多少量才发送,不然频繁的发送小数据不划算...当然如果只发一点点无法触发发送也是不行的，可以增加一个定时器操作,如果达到多少时间后数据量还是不够,就直接发送)
    // 当buf写空了以后才把EPOLLOUT事件去除。

    if (m_status == ConnStatus::DISCONNECTED || m_status == ConnStatus::DISCONNECTING) {
        LOG_ERROR("Connection != CONNECTED, can't send data");
        return;
    }

    // 1,先写入缓存中
    m_write_buf->Append(data, data_size);

    // 2,判断当前数据是否真的需要发送
    if (m_write_buf->GetReadableSize() > 0) {  // fixme：定时器+数据量，双重判断是否需要发送
        this->EnableWrite();
    }
}

void TcpConn::SendData(const std::string &data) {
    SendData(data.c_str(), data.size());
}

void TcpConn::ProcessReadEvent() {
    auto n = SocketOpt::ReadFdToBuffer(*m_read_buf, this->GetFD());
    if (n > 0) {
        // todo-hl：增加一个解包回调?
        // 应用层注入解析回调函数，由easynet回调,
        // if (m_owner->UpackStream(this->m_read_buf)) {
        //     m_owner->RecvMsg(shared_from_this());
        // }
        m_owner->RecvMsg(shared_from_this());
    } else if (n == 0) {
        // 当read返回0时,一定是表示对端关闭了连接,但是不知道是调用close还是shwtdown(SHUT_WR)
        // 如果只是本次数据传输完了,read会返回-1,并设置errno=EGAIN或EWOULDBLOCK,此时链接还继续保持.
        LOG_DEBUG("read=0 Connection has been disconnected");
        m_status = ConnStatus::DISCONNECTING;
        if (m_write_buf->GetReadableSize() == 0) {
            m_owner->DelConn(shared_from_this());
            m_status = ConnStatus::DISCONNECTED;
        }
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 对端没有数据可读
            return;
        }
        // 连接出现错误错误,删除链接
        LOG_ERROR("ReadFdToBuffer err:{}-{}", errno, strerror(errno));
        m_owner->DelConn(shared_from_this());
    }
}

void TcpConn::ProcessWriteEvent() {
    if (m_status == ConnStatus::DISCONNECTED) {
        LOG_ERROR("Connection has been disconnected");
        return;
    }

    // 此时对端可能是全链接or只读不写，那么继续给对端发送数据
    while (m_write_buf->GetReadableSize() != 0U) {
        auto ret = SocketOpt::WriteBufferToFd(*m_write_buf, this->GetFD());
        if (ret < 0) {
            // 写入失败,表示当前连接出现问题了，直接断开
            LOG_ERROR("WriteBufferToFd err:{}-{}", errno, strerror(errno));
            m_owner->DelConn(shared_from_this());
        }
        if (ret == 0) {
            // 当前写缓冲区已经满了,再次尝试写入
            break;
        }
    }

    // 根据剩余数据以及状态判断是否需要关闭写
    if (m_write_buf->GetReadableSize() == 0) {
        // 无数据可写了:
        // 如果对端关闭了写,那么就断开连接
        // 如果链接正常，则将EPOLLOUT事件删除，避免一直触发
        if (m_status == ConnStatus::DISCONNECTING) {
            LOG_DEBUG("Connection is disconnecting and no more data to send");
            m_owner->DelConn(shared_from_this());
            m_status = ConnStatus::DISCONNECTED;
            return;
        }
        this->DisableWrite();
        m_owner->WriteComplete(shared_from_this());
    }
}