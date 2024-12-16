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

AsyncHttpClient::AsyncHttpClient(std::shared_ptr<EventLoop> loop, const std::string url)
    : m_loop(loop) {
    // 0,初始化tcp_client
    m_client->onNewConnection = std::bind(&AsyncHttpClient::_on_connection, this, std::placeholders::_1);
    m_client->onRecvMsg = std::bind(&AsyncHttpClient::_on_recv_msg, this, std::placeholders::_1);
    m_client->onWriteComplete = std::bind(&AsyncHttpClient::_on_send_complete, this, std::placeholders::_1);
    m_client->onDelConnection = std::bind(&AsyncHttpClient::_on_del_connection, this, std::placeholders::_1);

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

void AsyncHttpClient::_on_connection(const TcpConnSPtr& conn) {
    // fixme：应该优化，当首次连接时，在这里发送，当连接仍然保持着时，应该在调用GET/POST时发送请求。
    // 是否维持连接同一个tcp连接应该交由上层来控制
    LOG_DEBUG("Get New Conn, Send: {}", m_req.SerializeToString());
    // 如果再这里发送请求，也只能发送一次，因为只会触发一次请求。(ps：还要考虑当请求断开时的问题)
    // 流程大概是这样子
    // connect -->服务端accept-->连接建立-->_on_connection-->首次发送请求-->服务端接收请求-->服务端处理请求-->服务端回包-->客户端接收回包-->_on_recv_msg-->cb
    // 剩余的请求都要在_on_recv_msg中发送
}

void AsyncHttpClient::_on_recv_msg(const TcpConnSPtr& conn) {
    LOG_DEBUG("Recv Msg: {}", conn->GetConnName());
    HttpContext ctx(http_parser_type::HTTP_RESPONSE);
    HttpResponse rsp;
    HttpContext::Status status = ctx.Parse(conn->GetBuffer().GetReadableAddr(), conn->GetBuffer().GetReadableSize(), rsp);
    if (status.ParseComplete()) {
        conn->GetBuffer().AdvanceReader(status.GetParsedLen());
    }
}

void AsyncHttpClient::_on_send_complete(const TcpConnSPtr& conn) {
    LOG_DEBUG("Sent Complete: {}", conn->GetConnName());
}

void AsyncHttpClient::_on_del_connection(const TcpConnSPtr& conn) {
    LOG_DEBUG("Remove Conn:{}", conn->GetConnName());
    // ps：当连接断开后,还有请求要发送时,应该重新建立连接
}

void AsyncHttpClient::AddRequest(const HttpRequestPtr& req, const CallBack& cb, double timeout) {
    // 应该将请求和回调函数保存起来，当连接建立时，发送请求
    // 如果请求的url一致，如果区分cb呢?
}

void AsyncHttpClient::Run() {
    // fixme：这个应该什么时候触发呢???
    m_client->connect();
}