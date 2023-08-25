#ifndef __EASYNET_HTTP_SERVER_H
#define __EASYNET_HTTP_SERVER_H

#include "http_request.h"
#include "http_response.h"
#include "tcp_server.h"

#include "buffer.h"
#include "cb.h"
#include "time_stemp.h"

class http_server {
 public:
    http_server(event_loop *loop, const std::string &ip, size_t port);

    void set_http_cb(const http_cb_f &f) { m_user_cb = f; }
    void start() { m_server.start(); }

 private:
    void _on_connection(const sp_tcp_connection_t &conn);
    int _on_msg(const sp_tcp_connection_t &conn, timestemp ts);
    void _on_request(const sp_tcp_connection_t &conn, const http_request &req);

 private:
    tcp_server m_server;
    http_cb_f m_user_cb;
};
#endif