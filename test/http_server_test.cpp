#include "cb.h"
#include "http_server.h"

int main()
{
    event_loop loop;
    http_server server(&loop, "127.0.0.1", 12345);
    server.set_http_cb([](const http_request&, http_response*) {
        // todo
    });
    server.set_thread_cnt(4);
    // server.set_error_cb();
    server.start();
}