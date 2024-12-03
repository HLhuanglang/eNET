#include "http_client.h"
#include "log.h"

int main() {
    EasyNet::LogInit(EasyNet::level::trace);
    EasyNet::HttpClient client("http://127.0.0.1:8888");

    // 同步阻塞接口
    // 0,调用Get接口
    // 1,tcp_client先执行connect
    // 2,执行loop
    // 3,在on_connection中发送数据：HttpRequest --> string
    // 4,在on_recv_msg中接收数据：string --> HttpResponse
    // 5,Get接口返回
    auto rsp = client.Get("/Hi");
    LOG_DEBUG("Recv rsp:\n{}", rsp.SerializeToString());
    return 0;
}