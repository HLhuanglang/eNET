#include "easy_net/cb.h"
#include "easy_net/http_server.h"

int main()
{
    event_loop loop;
    http_server server(&loop, "127.0.0.1", 12345);
    server.set_http_cb([](const http_request& req, http_response& rsp) {
        if (req.url_ == "/echo") {
            rsp.set_body(req.body_.c_str());
        }
    });
    server.start();
}