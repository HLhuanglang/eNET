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
    buf_ = nullptr;
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

    //做http数据完整性校验.
    //某一次http请求发生，客户端调用write后会将本次请求的所有数据都发送过来，服务端会全部read出来的！ 直到read返回-1 并且errno=EAGAIN才表示当前这次传输over了
    http_context* ctx = new http_context();
    http_request req;
    // auto ret = ctx->parser_http_context(buf->data_, buf->offset_, req); //传引用倒是不会引发拷贝构造,因为要修改,所以不加const
    // if (ret != buf->offset_) {
    //     //实际处理值和buf大小不一样,则说明http包不对.
    //     //断开链接
    // }

    _on_request(conn, req);
}

void http_server::_on_request(const tcp_connection& conn, const http_request& req)
{
    // todo
    //将用户要发送的数据转成buffer，然后发送出去
    http_response res;
    user_cb_(req, &res); //服务端根据客户端中的数据对res进行填充

    buffer buf;
    res.append_to_buffer(&buf); //将http_responese对象中的值转char数据进行发送.

    // conn->send_data(buf.data_, buf.offset_);
}