#ifndef __HOLO_SERVER_H
#define __HOLO_SERVER_H

#include "event_loop.h"
#include "tcp_server.h"
#include "holo_wo.h"

using holo_cb_f = std::function<void(const holo_wo_t&)>;

class holo_server {
public:
    holo_server(event_loop* loop, const std::string& ip, size_t port);

    void set_msg_cb();
    void start();

private:
    void _set_thread_cnt(size_t cnt) { server_.set_thread_cnt(cnt); }
    void _on_connection();
    void _on_msg(tcp_connection& conn, buffer& buf);
    void _on_request(tcp_connection& conn, holo_wo_t& holo_wo);

private:
    tcp_server server_;
    buffer* buf_;
    holo_cb_f user_cb_;
};

#endif