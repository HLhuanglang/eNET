#include "tcp_connection.h"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <string>

#include "socket_opt.h"

#include "buffer.h"
#include "log.h"

using namespace EasyNet;

void TcpConn::SendData(const char *data, size_t data_size) {
    // 这个函数主要是为了做异步发送，上层调用直接往buf里面写，不让上层阻塞在write上面
    // 上层调用的,只要判断buf中有数据就将EPOLLOUT事件添加到epoll,传入回调。在回调中调用write_buf_to_fd
    //(上层调用有个小优化逻辑,就是数据必须达到多少量才发送,不然频繁的发送小数据不划算...当然如果只发一点点无法触发发送也是不行的，可以增加一个定时器操作,如果达到多少时间后数据量还是不够,就直接发送)
    // 当buf写空了以后才把EPOLLOUT事件去除。

    // 1,先写入缓存中
    m_write_buf->Append(data, data_size);

    // 2,判断当前数据是否真的需要发送
    if (m_write_buf->GetReadableSize() > 0) { // fixme：定时器+数据量，双重判断是否需要发送
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
        // 当read返回0时,一定是表示对端关闭了连接
        // 如果只是本次数据传输完了,read会返回-1,并设置errno=EGAIN或EWOULDBLOCK,此时链接还继续保持.
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
        LOG_INFO("Connection has been disconnected");
        m_owner->DelConn(shared_from_this());
        return;
    }

    while (m_write_buf->GetReadableSize() != 0U) {
        auto ret = SocketOpt::WriteBufferToFd(*m_write_buf, this->GetFD());
        if (ret < 0) {
            // 写入失败,表示当前连接出现问题了，直接断开
            LOG_ERROR("WriteBufferToFd err");
            m_owner->DelConn(shared_from_this());
        }
        if (ret == 0) {
            // 当前写缓冲区已经满了,再次尝试写入
            break;
        }
    }
    if (m_write_buf->GetReadableSize() == 0) {
        // 无数据可写了,如果对端关闭了写,那么就断开连接
        // 如果链接正常，则将EPOLLOUT事件删除，避免一直触发
        if (m_status == ConnStatus::DISCONNECTING) {
            m_owner->DelConn(shared_from_this());
            m_status = ConnStatus::DISCONNECTED;
            return;
        }
        this->DisableWrite();
        m_owner->WriteComplete(shared_from_this());
    }
}