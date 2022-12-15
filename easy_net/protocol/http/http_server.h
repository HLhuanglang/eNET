#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

#include "buffer.h"
#include "cb.h"
#include "http_request.h"
#include "http_response.h"
#include "tcp_server.h"

class http_server {
public:
    using http_cb_f = std::function<void(const http_request&, http_response*)>;

    http_server(event_loop* loop, const std::string& ip, size_t port);

    void set_http_cb(const http_cb_f& f) { user_cb_ = f; }
    void set_error_cb();
    void set_thread_cnt(size_t cnt) { server_.set_thread_cnt(cnt); }
    void start() { server_.start(); }

private:
    void _on_connection();
    void _on_msg(const tcp_connection& conn, buffer* buf);
    void _on_request(const tcp_connection& conn, const http_request& req);

private:
    tcp_server server_;
    http_cb_f user_cb_;
    buffer* buf_;
};
#endif