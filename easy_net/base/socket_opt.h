#ifndef __EASYNET_SOCKET_OPT_H
#define __EASYNET_SOCKET_OPT_H

#include <cstddef>
#include <netinet/in.h>
#include <sys/socket.h>

#include "buffer.h"
#include "inet_addr.h"

namespace EasyNet {

class SocketOpt {
 public:
    /// @brief 将fd的接收缓冲区中的数据全部读取到buf中
    /// @param [out] buf 数据存储buf
    /// @param [in] fd 待读取数据的fd
    /// @return The return value description.
    ///     @retval n>0 实际读取值
    ///     @retval n=0 对端关闭了链接
    ///     @retval n=-1 接收错误(需要根据错误码进行处理)
    /// @note 读动作只需要读到数据即可,内部实现需要关注LT和ET模式
    /// LT：能读多少是多少
    /// ET：需要全部读
    static size_t ReadFdToBuffer(Buffer &buf, int fd);

    /// @brief 将发送缓冲区中的数据全部写入到fd中
    /// @param [out] buf 待发送数据数据
    /// @param [in] fd 被写入数据的fd
    /// @return The return value description.
    ///     @retval n>0 实际写入值
    ///     @retval n=0 由于fd发送缓冲区已满
    ///     @retval n=-1 发送错误
    /// @note
    /// 1,尽力写,不保证全部写入,需要上层调用者自己判断是否继续写入
    /// 2,buf内部会记录当前已成功写入的量
    static size_t WriteBufferToFd(Buffer &buf, int fd);

    /// @brief 将fd设置程非阻塞模式
    /// @return true-设置成功 flase-设置失败
    /// @note
    /// linux平台下调用socket默认都是阻塞模式
    static bool SetFdNonblock(int fd);

    /// @brief 防止fd在子进程中意外地被继承
    /// @return true-设置成功 flase-设置失败
    static bool SetFdCloseOnExec(int fd);

    /// @brief 设置端口复用
    /// @return ture-设置成功 false-设置失败
    /// @note 允许一个端口被多个套接字bind
    /// 防止服务器重启时,之前绑定的端口还未释放或者程序突然退出而系统没有释放端口
    /// 这种情况下如果设定了端口复用，则新启动的服务器进程可以直接绑定端口
    /// 如果没有设定端口复用，绑定会失败，提示ADDR已经在使用中
    static bool SetReuseAddr(int fd);

    /// @brief 设置端口重用
    /// @return ture-设置成功 false-设置失败
    /// @note 解决多进程下accept负载不均衡的问题
    static bool SetReusePort(int fd);

    ///@brief 设置tcp的keep-alive状态
    ///@param on 是否开启keep-alive
    ///@param fd 待操作的套接字
    static void SetKeepAlive(int fd, bool on);

    ///@brief 获取本机地址
    ///@return 网络地址结构体
    static struct sockaddr_in6 GetLocalAddr(int fd);

    ///@brief 获取对端的地址
    ///@return 网络地址结构体
    static struct sockaddr_in6 GetPeerAddr(int fd);

    ///@brief 创建非阻塞的fd
    ///@param family 协议族(ipv4 or ipv6)
    ///@return 非阻塞fd
    static int CreateNonBlockSocket(sa_family_t family);

    ///@brief 获取当前fd的错误码
    ///@param fd 待处理的fd
    ///@return 非0存在错误,表示具体的错误码
    ///@note 使用strerror可以获取到错误信息
    static int GetSocketError(int fd);

    static int Listen(int fd, size_t queue_size);

    static bool IsSelfConnect(int fd);

    static int Connect(int fd, const struct sockaddr *addr);

    static int Accept(int fd, InetAddress &perrAddr);

    static void Close(int fd);

    static void ShutDownWrite(int fd);
};
} // namespace EasyNet

#endif