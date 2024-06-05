#include "http_server.h"

#include "http_context.h"
#include "http_request.h"
#include "http_response.h"
#include "log.h"
#include "tcp_server.h"

using namespace EasyNet;

HttpServer::HttpServer(const std::string &ip, size_t port)
    : m_server("httpsvr", KThreadPoolSize, {ip.c_str(), static_cast<uint16_t>(port)}) {
    m_server.onNewConnection = std::bind(&HttpServer::_on_connection, this, std::placeholders::_1);
    m_server.onRecvMsg = std::bind(&HttpServer::_on_recv_msg, this, std::placeholders::_1);
    m_server.onWriteComplete = std::bind(&HttpServer::_on_send_complete, this, std::placeholders::_1);
    m_server.onDelConnection = std::bind(&HttpServer::_on_del_connection, this, std::placeholders::_1);
}

void HttpServer::_on_connection(const tcp_connection_t &conn) {
    LOG_DEBUG("Get New Conn");
}

void HttpServer::_on_recv_msg(const tcp_connection_t &conn) {
    LOG_DEBUG("Recv Msg: {}", conn->GetConnName());
    HttpContext ctx;
    HttpRequest req;
    // PS：buff里面的数据有多种情况：
    // 1,完整的http请求
    // 2,不完整的http请求
    // 3,多个http请求
    HttpContext::Status status = ctx.Parse(conn->GetBuffer().GetReadableAddr(), conn->GetBuffer().GetReadableSize(), req);
    if (status.ParseComplete()) {
        // 1,解析成功,那么消费掉对应的数据
        conn->GetBuffer().AdvanceReader(status.GetParsedLen());

        // 2,根据url找到对应的处理函数
        HttpResponse resp;
        m_router.Routing(req, resp);

        // 3,回包给客户端
        conn->SendData(resp.SerializeToString());
    }
}

void HttpServer::_on_send_complete(const tcp_connection_t &conn) {
    LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
}

void HttpServer::_on_del_connection(const tcp_connection_t &conn) {
    LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
}

void HttpServer::Run(const HttpRouter &router) {
    m_router = router;
    m_server.start();
}