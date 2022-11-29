#include "event_loop.h"
#include "tcp_connection.h"
#include "tcp_server.h"

int main()
{
    event_loop loop;
    tcp_server server(&loop, "0.0.0.0", 1234);

    server.set_recv_msg_cb([](tcp_connection *conn, buffer *buf) {
        printf("recv client req!\n");
        conn->send_data(buf->data_, buf->offset_);
    });
    server.set_build_connection_cb([]() { printf("build connection!"); });
    server.set_close_connection_cb([]() { printf("connection closed!"); });
    server.set_thread_cnt(4);
    server.start();
    return 0;
}