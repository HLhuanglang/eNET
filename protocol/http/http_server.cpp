#include "http_server.h"
#include <functional>
#include <memory>

#include "http_context.h"
#include "tcp_connection.h"

#include "buffer.h"
#include "def.h"

http_server::http_server(event_loop *loop, const std::string &ip, size_t port)
    : m_server(loop, ip.c_str(), port),
      m_user_cb([](const http_request &req, http_response &res) {
          // 默认情况下的响应
          res.set_status_code("404");
          res.set_status_code_msg("Not Found");
          res.set_close_connection(true);
      }) {

    // 绑定回调接口
    m_server.set_new_connection_cb(std::bind(&http_server::_on_connection, this, std::placeholders::_1));
    m_server.set_recv_msg_cb(std::bind(&http_server::_on_msg, this, std::placeholders::_1, std::placeholders::_2));
}

void http_server::_on_connection(const sp_tcp_connection_t &conn) {
    // fixme：
    // if (conn->connected()) {
    //     http_context *ctx = new http_context;
    //     conn->setContext((void *)ctx);
    // }
}

int http_server::_on_msg(const sp_tcp_connection_t &conn, timestemp ts) {
    // 做http数据完整性校验.
    // 某一次http请求发生，客户端调用write后会将本次请求的所有数据都发送过来，服务端会全部read出来的！ 直到read返回-1 并且errno=EAGAIN才表示当前这次传输over了
    auto sp_ctx = std::make_shared<http_context>(); // 频繁的创建对象也是一种开销
    http_request req;
    auto parsed_size = sp_ctx->parser_http_context(conn->get_readbuf().readable_start(), conn->get_readbuf().readable_size(), req); // fixme：这里会从buf拷贝数据...,可否直接使用buf中的数据而避免copy呢?
    if (sp_ctx->is_http_complete()) {
        // 不能收到一部分数据就解析一部分数据，并且该部分数据被解析完后还存储在http_server对象中(诸如请求方法、协议版本等信息)
        // 因为http_server要处理很多的请求，可能同时有多个连接在收发数据，这样子http_server中保存的解析完成的数据就混乱了
        //
        // 上面的想法存在问题
        //
        // 两种方案：原理都是状态机
        //
        // 方案1：保存解析状态，将状态传入状态机中。tcp_connection中保留一个context变量，用来存储http协议对象，这样子http协议对象就和某条具体的tcp连接关联上了
        //  这样每次收到数据都做一波解析，并且记录当前的状态，然后下一次就能减少前面的重复解析步骤，当前这次解析完成后就在buffer中删掉此次解析的数据
        //
        // 方案2：不保存解析状态，状态机每次都要重启。tcp_connection只提供原始数据
        //  这个方案就是每次收到数据都做一次解析，存在重复解析的问题。当解析完成发现是一个完整的http请求后，才返回给调用者，然后才会把数据从buffer中删除。
        //
        // 两种方案谁更优呢？
        // 方案1，接收到一部分数据就解析一部分，然后释放一部分空间，虽说buffer中的数据被释放了，但是context对象又重新占用了内存呀~
        // 方案2看来来多了重复解析以及占用buffer空间的问题，但是http协议的头部其实也没多少字节。问题在于重复解析时，由于使用http_parser会调用一大堆的回调，然后进行string的拷贝操作。
        // 完事如果此次接收不完整的话，这一套赋值操作就白费了。
        //
        // 还是采用方案1比较好点？自己实现一套http协议的解析即可。

        conn->get_readbuf().clear(); // fixme：因为已经解析完了，所以将buf清空？

        _on_request(conn, req);
    } else {
        // 收包不完整,继续收
    }
    return static_cast<int>(parsed_size);
}

void http_server::_on_request(const sp_tcp_connection_t &conn, const http_request &req) {
    // 将用户要发送的数据转成buffer，然后发送出去
    http_response res;
    res.default_init();
    m_user_cb(req, res); // 服务端根据客户端中的数据对res进行填充

    http_response::append_http_to_buf(res, conn->get_writebuf()); // 将http_responese对象中的值转char数据进行发送.
    conn->send_data(conn->get_writebuf().readable_start(), conn->get_writebuf().readable_size());
}