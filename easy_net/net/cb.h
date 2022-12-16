#ifndef __CB_H
#define __CB_H

#include <functional>

// Pre-declaration
class event_loop;
class tcp_connection;
class buffer;
class http_request;
class http_response;

using event_cb_f    = std::function<void(event_loop *loop, int fd, void *args)>;
using task_cb_f     = std::function<void(event_loop *loop, void *args)>;
using recv_msg_cb_f = std::function<void(tcp_connection &conn, buffer &buf)>;
using http_cb_f     = std::function<void(const http_request &, http_response &)>;

#endif