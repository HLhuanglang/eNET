/*
    http客户端的api设计可以参考cpp-httplib,非常简单实用.
*/
#ifndef __EASYNET_HTTP_CLIENT_H
#define __EASYNET_HTTP_CLIENT_H

#include <cstddef>

#include "http_request.h"
#include "http_response.h"
#include "tcp_client.h"

#include "cb.h"

class http_client {
 public:
    http_client(const std::string &ip, size_t port);

 private:
    http_request m_req;
};

#endif