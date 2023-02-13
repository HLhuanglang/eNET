#ifndef __EASYNET_CONNECTION_OWNER_H
#define __EASYNET_CONNECTION_OWNER_H

#include <cstddef>
#include <string>

#include "buffer.h"
#include "cb.h"
class tcp_connection;

class connection_owner {
public:
    connection_owner() = default;
    virtual ~connection_owner() = default;

public:
    virtual void new_connection(int fd, std::string ip, std::string port) = 0;
    virtual void del_connection(const sp_tcp_connectopn_t &conn) = 0;
    virtual void recv_msg(const sp_tcp_connectopn_t &conn, buffer &buf) = 0;
    virtual void write_complete(const sp_tcp_connectopn_t &conn) = 0;
    virtual void high_water_mark(const sp_tcp_connectopn_t &conn, size_t mark) = 0;
    virtual event_loop *get_loop() const = 0;
};

#endif