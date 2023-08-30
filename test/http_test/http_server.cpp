#include "easy_net/http_server.h"
#include "easy_net/event_loop.h"

int main() {
    event_loop loop;
    http_server server(&loop, "127.0.0.1", 6666);
    server.set_http_cb([](const http_request &req, http_response &resp) {
        if (req.m_url == "/hello") {
            resp.set_status_code(http_response::status_code_e::OK);
            resp.set_body("hello world");
        } else {
            resp.set_status_code(http_response::status_code_e::NOT_FOUND);
            resp.set_body("404 not found");
        }
    });
    server.start();
    return 0;
}