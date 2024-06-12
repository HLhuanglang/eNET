#include "http_router.h"
#include "http_server.h"
#include "log.h"

int main() {
    EasyNet::LogInit(EasyNet::level::debug);
    EasyNet::HttpRouter route;
    route.GET("/Hi", [](const EasyNet::HttpRequest &req, EasyNet::HttpResponse &res) {
        // 1,处理请求
        LOG_DEBUG("Recv Request:\n{}", req.SerializeToString());

        // 2,回包
        res.SetStatusCode("200");
        res.SetStatusCodeMsg("OK");
        res.SetBody("Hello, World!");
    });

    EasyNet::HttpServer svr("0.0.0.0", 8888);
    svr.Run(route);
}