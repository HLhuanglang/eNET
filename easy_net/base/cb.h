#ifndef __EASYNET_CB_H
#define __EASYNET_CB_H

#include <cstddef>
#include <functional>
#include <memory>

// Pre-declaration
class event_loop;
class tcp_connection;
class buffer;
class http_request;
class http_response;

using event_cb_f = std::function<void(event_loop *loop, int fd, void *args)>;
using http_cb_f = std::function<void(const http_request &, http_response &)>;
using sp_tcp_connectopn_t = std::shared_ptr<tcp_connection>;

#endif