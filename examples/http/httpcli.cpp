#include <unistd.h>

#include <memory>

#include "event_loop.h"
#include "http_client.h"
#include "http_request.h"
#include "log.h"

int main() {
    EasyNet::LogInit(EasyNet::level::trace);

    // 同步http客户端
    // EasyNet::SyncHttpClient sync_client("http://127.0.0.1:8888");
    // auto rsp1 = sync_client.Get("/Hi");
    // LOG_DEBUG("Recv rsp1:\n{}", rsp1.SerializeToString());
    // auto rsp2 = sync_client.Get("/Hello");
    // LOG_DEBUG("Recv rsp1:\n{}", rsp2.SerializeToString());

    // 异步http客户端
    auto loop = std::make_shared<EasyNet::EventLoop>("async_http_cli");
    EasyNet::AsyncHttpClient async_client(loop, "http://127.0.0.1:8888");
    auto get_req = std::make_shared<EasyNet::HttpRequest>();
    get_req->m_method = "GET";
    get_req->m_url = "/Hi";
    async_client.AddRequest(get_req, [](const EasyNet::HttpResponse &rsp) {
        LOG_DEBUG("Recv rsp:\n{}", rsp.SerializeToString());
    });
    auto post_req = std::make_shared<EasyNet::HttpRequest>();
    post_req->m_method = "POST";
    post_req->m_url = "/Hello";
    post_req->m_body = "Hello, World!";
    post_req->m_headers.SetHeader("Content-Length", std::to_string(post_req->m_body.size()));
    post_req->m_headers.SetHeader("Content-Type", "text/plain");
    async_client.AddRequest(post_req, [](const EasyNet::HttpResponse &rsp) {
        LOG_DEBUG("Recv rsp:\n{}", rsp.SerializeToString());
    });
    async_client.Run();
    loop->Loop();

    return 0;
}