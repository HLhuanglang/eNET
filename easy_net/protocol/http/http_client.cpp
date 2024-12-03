#include "http_client.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>

#include "dns.h"
#include "event_loop.h"
#include "http_context.h"
#include "http_parser.h"
#include "http_request.h"
#include "http_response.h"
#include "http_url.h"
#include "inet_addr.h"
#include "log.h"

using namespace EasyNet;

HttpClient::HttpClient(const std::string url) : HttpClient(std::make_shared<EventLoop>("httpcli_loop"), url) {
    m_client->onNewConnection = std::bind(&HttpClient::_on_connection, this, std::placeholders::_1);
    m_client->onRecvMsg = std::bind(&HttpClient::_on_recv_msg, this, std::placeholders::_1);
    m_client->onWriteComplete = std::bind(&HttpClient::_on_send_complete, this, std::placeholders::_1);
    m_client->onDelConnection = std::bind(&HttpClient::_on_del_connection, this, std::placeholders::_1);
}

HttpClient::HttpClient(std::shared_ptr<EventLoop> loop, const std::string url)
    : m_loop(loop) {
    // 1,解析schema
    HttpUrlParser parser(url);
    assert(parser.Parse());

    // 2,如果是域名,使用dns查询ip地址
    std::vector<InetAddress> tmp_addr_v;
    auto resolver = DNSResolver::CreateResolver();
    resolver->resolve(parser.url.host, [&tmp_addr_v](const std::vector<InetAddress>& addr) {
        tmp_addr_v.resize(addr.size());
        std::copy(addr.begin(), addr.end(), tmp_addr_v.begin());
    });

    // 2,创建tcp_client
    if (!tmp_addr_v.empty()) {
        InetAddress tmp_addr = *tmp_addr_v.begin();
        tmp_addr.setPort(std::atoi(parser.url.port.c_str()));
        m_client = make_unique<TcpClient>(loop.get(), tmp_addr);
    } else {
        LOG_ERROR("Failed to resolve hostname: {}", parser.url.host);
        exit(EXIT_FAILURE);
    }
}

void HttpClient::_on_connection(const TcpConnSPtr& conn) {
    LOG_DEBUG("Get New Conn, Send: {}", m_req.SerializeToString());
    conn->SendData(m_req.SerializeToString());
}

void HttpClient::_on_recv_msg(const TcpConnSPtr& conn) {
    LOG_DEBUG("Recv Msg: {}", conn->GetConnName());
    HttpContext ctx(http_parser_type::HTTP_RESPONSE);
    HttpContext::Status status = ctx.Parse(conn->GetBuffer().GetReadableAddr(), conn->GetBuffer().GetReadableSize(), m_rsp);
    if (status.ParseComplete()) {
        conn->GetBuffer().AdvanceReader(status.GetParsedLen());
        conn->GetOwnerLoop()->Quit();  // fixme：研究一下这样实现是否正常.
    }
}

void HttpClient::_on_send_complete(const TcpConnSPtr& conn) {
    LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
}

void HttpClient::_on_del_connection(const TcpConnSPtr& conn) {
    LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
}

HttpResponse HttpClient::Get(const std::string& url) {
    m_req.m_method = "GET";
    m_req.m_url = url;
    m_client->connect();
    m_client->GetEventLoop()->Loop();  // 阻塞等待接收服务器的消息
    return m_rsp;
}

HttpResponse HttpClient::Post(const std::string& url, const HttpRequest& req) {
    // TODO
    m_req.m_method = "POST";
    m_req.m_url = url;
    m_client->connect();
    m_client->GetEventLoop()->Loop();  // 阻塞等待接收服务器的消息
    return m_rsp;
}