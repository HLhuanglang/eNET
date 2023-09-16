#include "tcp_server.h"
#include "inet_addr.h"

tcp_server::tcp_server(event_loop *loop, const char *ip, size_t port)
    : m_main_loop(loop), m_addr(inet_addr::IPV4, ip, port) {
    // todo
}

void tcp_server::start(int thread_cnt) {
    // todo
}

void tcp_server::new_connection(int fd, std::string ip, std::string port) {
}
void tcp_server::del_connection(const sp_tcp_connectopn_t &conn) {}
void tcp_server::recv_msg(const sp_tcp_connectopn_t &conn) {}
void tcp_server::write_complete(const sp_tcp_connectopn_t &conn) {}
void tcp_server::high_water_mark(const sp_tcp_connectopn_t &conn, size_t mark) {}
event_loop *tcp_server::get_loop() const {
    return m_main_loop;
}