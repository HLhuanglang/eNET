#ifndef __EASYNET_HTTP_SERVER_H
#define __EASYNET_HTTP_SERVER_H

#include "http_request.h"
#include "http_response.h"
#include "tcp_server.h"

#include "buffer.h"
#include "cb.h"

class http_server {
public:
    http_server(event_loop *loop, const std::string &ip, size_t port);

    void set_http_cb(const http_cb_f &f) { user_cb_ = f; }
    void start() { server_.start(); }

private:
    void _on_connection();
    int _on_msg(tcp_connection &conn, buffer &buf);
    void _on_request(tcp_connection &conn, const http_request &req);

private:
    tcp_server server_;
    http_cb_f user_cb_;
    buffer *buf_;
};
#endif