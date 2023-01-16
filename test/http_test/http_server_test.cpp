#include <cstdio>

#include "easy_net/cb.h"
#include "easy_net/http_server.h"

int main() {
    event_loop loop;
    http_server server(&loop, "0.0.0.0", 12345);
    server.set_http_cb([](const http_request &req, http_response &rsp) {
        if (req.url_ == "/hi") {
            printf("req.body_size:%ld\n", req.body_.size());
            for (auto &n : req.headers_) {
                printf("%s:%s\n", n.first.c_str(), n.second.c_str());
            }
            rsp.set_body("Hello,world!");
        }
    });
    server.start();
}