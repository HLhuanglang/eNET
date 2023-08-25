#ifndef __EASYNET_CONNECTION_OWNER_H
#define __EASYNET_CONNECTION_OWNER_H

#include <cstddef>
#include <functional>
#include <string>

#include "buffer.h"
#include "cb.h"
#include "time_stemp.h"

class connection_owner {
 public:
    connection_owner() = default;
    virtual ~connection_owner() = 0;

 public:
    // 新建连接
    virtual void new_connection(int fd, std::string ip, std::string port) = 0;

    // 断开连接
    virtual void del_connection(const sp_tcp_connectopn_t &conn) = 0;

    // 收到消息
    virtual void recv_msg(const sp_tcp_connectopn_t &conn) = 0;

    // 发送缓冲区数据已写入内核(不一定发送到对端)
    virtual void write_complete(const sp_tcp_connectopn_t &conn) = 0;

    // 发送缓冲区高水位
    virtual void high_water_mark(const sp_tcp_connectopn_t &conn, size_t mark) = 0;

    // 获取当前连接属于哪个loop
    virtual event_loop *get_loop() const = 0;
};

#endif