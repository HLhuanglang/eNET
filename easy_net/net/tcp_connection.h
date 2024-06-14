#ifndef __EASYNET_TCP_CONNECTION_H
#define __EASYNET_TCP_CONNECTION_H

#include <cstddef>
#include <memory>

#include "buffer.h"
#include "connection_owner.h"
#include "inet_addr.h"
#include "io_event.h"

namespace EasyNet {

// 前置声明
class EventLoop;

class TcpConn : public std::enable_shared_from_this<TcpConn>, public IOEvent {
 public:
    enum class ConnStatus {
        CONNECTING,     // 正在连接
        CONNECTED,      // 已经连接
        DISCONNECTING,  // 正在断开连接(给客户端使用的,就是只关闭写端)
        DISCONNECTED,   // 断开连接
    };

 public:
    ///@brief 构造函数
    ///@param owner 当前这条tcp连接的拥有者，可以是tcpclient或者tcpserver
    ///@param fd 当前这条tcp连接的文件描述符
    ///@param perrAddr 对端地址
    TcpConn(ConnOwner *owner, int fd, const InetAddress &perrAddr);

    ///@brief 析构函数
    ~TcpConn() override;

 public:
    ///@brief 延长tcp连接的生命周期
    ///@note 为了防止恶意客户端一直占据连接,服务端需要控制tcp连接的生命周期,当连接闲置(无数据收发)时,服务端主动断开连接
    void KeepAlive();

    ///@brief 发送数据
    ///@param data 指向存放数据的指针
    ///@param data_size 待发送的数据大小
    void SendData(const char *data, size_t data_size);

    ///@brief 发送数据
    ///@param data 待发送的数据,使用std::string进行存储
    void SendData(const std::string &data);

    ///@brief 获取读缓存
    ///@return 返回当前tcp连接中的读缓存(可读数据)
    Buffer &GetBuffer() { return *m_read_buf; }

    ///@brief 获取当前这条tcp连接的名字
    ///@return 连接名字
    std::string GetConnName() { return m_name; }

    ///@brief 设置当前tcp连接的状态
    ///@param status tcp状态枚举
    ///@see ConnStatus
    void SetStatus(ConnStatus status) { m_status = status; }

    ///@brief 获取当前这条tcp连接属于哪一个loop
    ///@return 所属的loop
    EventLoop *GetOwnerLoop() { return m_ioloop; }

 public:
    void ProcessWriteEvent() override;
    void ProcessReadEvent() override;

 private:
    ConnOwner *m_owner;   // tcpclient或tcpserver会传入this指针，此时owner_指向tcpclient或tcpserver
    Buffer *m_read_buf;   // 读buffer(存储对端发送给本端的数据)
    Buffer *m_write_buf;  // 写buffer(存储本端将要发送给对端的数据)
    std::string m_name;   // 连接的name
    ConnStatus m_status;  // 连接的状态
};

}  // namespace EasyNet

#endif