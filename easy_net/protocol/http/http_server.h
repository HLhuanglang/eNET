#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

#include "http_router.h"
#include "tcp_server.h"

namespace EasyNet {
class HttpServer {
 public:
    HttpServer(const std::string &ip, size_t port);

 public:
    void Run(const HttpRouter &router);

 private:
    void _on_connection(const tcp_connection_t &conn);
    void _on_recv_msg(const tcp_connection_t &conn);
    void _on_send_complete(const tcp_connection_t &conn);
    void _on_del_connection(const tcp_connection_t &conn);

 private:
    TcpServer m_server;
    HttpRouter m_router;
};
}  // namespace EasyNet

#endif