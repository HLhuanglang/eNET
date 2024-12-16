#include "http_router.h"
#include "http_server.h"
#include "log.h"

int main() {
    EasyNet::LogInit(EasyNet::level::trace);
    EasyNet::HttpRouter route;
    route.GET("/Hi", [](const EasyNet::HttpRequest &req, EasyNet::HttpResponse &res) {
        // 1,处理请求
        LOG_DEBUG("Recv Request:\n{}", req.SerializeToString());

        // 2,回包
        res.SetStatusCode("200");
        res.SetStatusCodeMsg("OK");
        res.SetBody("Hi!");
    });
    route.POST("/Hello", [](const EasyNet::HttpRequest &req, EasyNet::HttpResponse &res) {
        // 1,处理请求
        LOG_DEBUG("Recv Request:\n{}", req.SerializeToString());

        // fixme
        // 如果在这里需要发送http请求其他服务,应该怎么做?
        // 嵌套请求怎么处理比较优雅？ AB、AC、AD
        // cli -->[A --> B --> C --> D]--> cli

        // 2,回包
        res.SetStatusCode("200");
        res.SetStatusCodeMsg("OK");
        res.SetBody("Hello!");
    });

    EasyNet::HttpServer svr("127.0.0.1", 8888);
    svr.Run(route);
}