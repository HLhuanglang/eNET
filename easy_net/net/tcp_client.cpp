#include "tcp_client.h"
#include "def.h"

using namespace EasyNet;

CallBack tcp_client::m_new_connection_cb;
CallBack tcp_client::m_del_connection_cb;
CallBack tcp_client::m_revc_msg_cb;
CallBack tcp_client::m_write_complete_cb;

tcp_client::tcp_client(EventLoop *loop, const InetAddress &addr)
    : m_loop(loop) {
    // todo
}