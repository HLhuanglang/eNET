#include "easy_net/cb.h"
#include "easy_net/http_server.h"
#include <bits/types/FILE.h>
#include <cstdio>

int main()
{
    event_loop loop;
    http_server server(&loop, "0.0.0.0", 12345);
    server.set_http_cb([](const http_request& req, http_response& rsp) {
        if (req.url_ == "/echo") {
            FILE* fp = fopen("./test.jpg", "wb+");
            fwrite(req.body_.c_str(), 1, req.body_.size(), fp);
            printf("req.body_size:%ld\n", req.body_.size());
            printf("req.methon:%s\n", req.method_.c_str());
            printf("req.version:%s\n", req.version_.c_str());
            for (auto& n : req.headers_) {
                printf("%s:%s\n", n.first.c_str(), n.second.c_str());
            }
            rsp.set_body("Hello,world!");
        }
    });
    server.start();
}