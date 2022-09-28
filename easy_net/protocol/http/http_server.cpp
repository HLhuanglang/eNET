#include "http_server.h"
#include "http_context.h"
#include <functional>

http_server::http_server(event_loop* loop, const std::string& ip, size_t port)
    : server_(loop, ip.c_str(), port)
    , user_cb_([](const http_request& req, http_response* res) {
        //默认情况下的响应
        res->set_status_code("404");
        res->set_status_code_msg("Not Found");
        res->set_close_connection(true);
    })
{
    // server_.set_build_connection_cb(std::bind(&http_server::_on_connection, this));
    // server_.set_recv_msg_cb(std::bind(&http_server::_on_msg, this, std::placeholders::_1, std::placeholders::_2));
}

void http_server::_on_connection()
{
    // todo
}

void http_server::_on_msg(const tcp_connection& conn, buffer* buf)
{
    // todo
    //收到客户端发送的消息了,然后进行处理,将处理完成的数据给_on_quest使用,存放在req中.

    http_context* ctx = new http_context();
    http_request req;
    ctx->parser_http_context(buf->data_, buf->offset_, req); //传引用倒是不会引发拷贝构造,因为要修改,所以不加const

    _on_request(conn, req);
}

void http_server::_on_request(const tcp_connection& conn, const http_request& req)
{
    // todo
    //将用户要发送的数据转成buffer，然后发送出去
    http_response res;
    user_cb_(req, &res); //服务端根据客户端中的数据对res进行填充

    buffer buf(1024);
    res.append_to_buffer(&buf); //将http_responese对象中的值转char数据进行发送.

    // conn->send_data(buf.data_, buf.offset_);
}