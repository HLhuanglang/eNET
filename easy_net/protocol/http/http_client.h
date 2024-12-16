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
///@brief 异步http客户端
///@note 需要由外部传入event_loop
class AsyncHttpClient {
 public:
    using CallBack = std::function<void(const HttpResponse &rsp)>;

    ///@brief 使用外部传入的event_loop构建http_client
    ///@param loop 事件循环
    ///@param url 请求的url
    AsyncHttpClient(std::shared_ptr<EventLoop> loop, const std::string url);

    ~AsyncHttpClient() = default;

    void AddRequest(const HttpRequestPtr &req, const CallBack &cb, double timeout = 0.0);
    void Run();

 private:
    void _on_connection(const TcpConnSPtr &conn);
    void _on_recv_msg(const TcpConnSPtr &conn);
    void _on_send_complete(const TcpConnSPtr &conn);
    void _on_del_connection(const TcpConnSPtr &conn);

 private:
    HttpHeaders m_headers;
    HttpRequest m_req;
    std::shared_ptr<EventLoop> m_loop;
    std::unique_ptr<TcpClient> m_client;
};

///@brief 同步http客户端
///@note [GET/POST/...]-> connect -> send -> recv -> http_parser -> HttpResponse
class SyncHttpClient {
 public:
    SyncHttpClient(const std::string url);

    ~SyncHttpClient() = default;

 private:
};

}  // namespace EasyNet

#endif  // !__EASYNET_HTTP_CLIENT_H
