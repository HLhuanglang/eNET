#include "http_server.h"
#include "http_context.h"
#include <functional>
#include <memory>
#include "tcp_connection.h"
#include "buffer.h"
#include "print_debug.h"

http_server::http_server(event_loop* loop, const std::string& ip, size_t port)
    : server_(loop, ip.c_str(), port)
    , user_cb_([](const http_request& req, http_response& res) {
        //默认情况下的响应
        res.set_status_code("404");
        res.set_status_code_msg("Not Found");
        res.set_close_connection(true);
    })
{
    _set_thread_cnt(4); //fixme：这样子肯定是不行的.
    buf_ = nullptr;
    server_.set_build_connection_cb(std::bind(&http_server::_on_connection, this));
    server_.set_recv_msg_cb(std::bind(&http_server::_on_msg, this, std::placeholders::_1, std::placeholders::_2));
}

void http_server::_on_connection()
{
    // todo
}

void http_server::_on_msg(tcp_connection& conn, buffer& buf)
{
    //做http数据完整性校验.
    //某一次http请求发生，客户端调用write后会将本次请求的所有数据都发送过来，服务端会全部read出来的！ 直到read返回-1 并且errno=EAGAIN才表示当前这次传输over了
    auto sp_ctx = std::make_shared<http_context>();
    http_request req;
    sp_ctx->parser_http_context(buf.readble_start(), buf.readable_size(), req); //fixme：这里会从buf拷贝数据...,可否直接使用buf中的数据而避免copy呢?
    if (sp_ctx->is_http_complete()) {
        _on_request(conn, req);
    } else {
        //收包不完整,继续收
    }
}

void http_server::_on_request(tcp_connection& conn, const http_request& req)
{
    // 将用户要发送的数据转成buffer，然后发送出去
    http_response res;
    res.default_init();
    user_cb_(req, res); //服务端根据客户端中的数据对res进行填充

    http_response::append_http_to_buf(res, conn.get_writebuf()); //将http_responese对象中的值转char数据进行发送.
    conn.send_data(conn.get_writebuf().readble_start(), conn.get_writebuf().readable_size());
}