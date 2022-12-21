#include "holo_server.h"
#include "http_server.h"
#include "holo_wo.h"

holo_server::holo_server(event_loop* loop, const std::string& ip, size_t port)
    : server_(loop, ip.c_str(), port)
{
    _set_thread_cnt(4); //fixme：这样子肯定是不行的.
    buf_ = nullptr;
    server_.set_build_connection_cb(std::bind(&holo_server::_on_connection, this));
    server_.set_recv_msg_cb(std::bind(&holo_server::_on_msg, this, std::placeholders::_1, std::placeholders::_2));
}

void holo_server::_on_connection()
{
    //todo
}

void holo_server::_on_msg(tcp_connection& conn, buffer& buf)
{
    //buf 收到的数据包
    holo_wo_t pkg;
    auto ret = encode_holo_wo_pkg(buf.readble_start(), buf.readable_size(), pkg);
    if (ret == encode_state::SUCCESS) {
        //拆包完成了,给用户使用
        _on_request(conn, pkg);
    }
}

void holo_server::_on_request(tcp_connection& conn, holo_wo_t& holo_wo)
{
    user_cb_(holo_wo);
}