/*
    http客户端的api设计可以参考cpp-httplib,非常简单实用.
*/
#ifndef __HTTP_CLIENT_H
#define __HTTP_CLIENT_H

#include <cstddef>

#include "http_request.h"
#include "http_response.h"
#include "tcp_client.h"

#include "cb.h"

class http_client {
public:
    http_client(const std::string &ip, size_t port);

private:
    http_request req_;
    tcp_client::after_connection_cb_f after_conn_cb_;
};

#endif