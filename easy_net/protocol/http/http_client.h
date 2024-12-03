#ifndef __EASYNET_HTTP_CLIENT_H
#define __EASYNET_HTTP_CLIENT_H

#include <memory>
#include <string>

#include "event_loop.h"
#include "http_headers.h"
#include "http_request.h"
#include "http_response.h"
#include "tcp_client.h"

namespace EasyNet {

class HttpClient {
 public:
    ///@brief 使用外部传入的event_loop构建http_client
    HttpClient(std::shared_ptr<EventLoop> loop, const std::string url);

    ///@brief 使用内部创建的event_loop构建http_client
    HttpClient(const std::string url);

    ~HttpClient() = default;

    HttpResponse Get(const std::string &url);

    HttpResponse Post(const std::string &url, const HttpRequest &req);

 private:
    void _on_connection(const TcpConnSPtr &conn);
    void _on_recv_msg(const TcpConnSPtr &conn);
    void _on_send_complete(const TcpConnSPtr &conn);
    void _on_del_connection(const TcpConnSPtr &conn);

 private:
    HttpHeaders m_headers;
    HttpRequest m_req;
    HttpResponse m_rsp;
    std::shared_ptr<EventLoop> m_loop;
    std::unique_ptr<TcpClient> m_client;
};
}  // namespace EasyNet

#endif  // !__EASYNET_HTTP_CLIENT_H
