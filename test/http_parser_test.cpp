#include "easy_net/buffer.h"
#include "easy_net/http_context.h"
#include "easy_net/http_response.h"

#include <iostream>

int main()
{
    std::string str_http_req = "";
    str_http_req += "POST /uploaddata HTTP/1.1\r\n";
    str_http_req += "Host: 127.0.0.1:8080\r\n";
    str_http_req += "Accept: */*\r\n";
    str_http_req += "Content-Length: 70\r\n";
    str_http_req += "Content-Type: application/json\r\n";
    str_http_req += "\r\n";
    str_http_req += "{\"DeviceID\":\"12345678\",\"LowPressure\":80,\"HighPressure\":120,\"Pulse\":90}";

    std::string str_http_rsp = "";
    str_http_rsp += "HTTP/1.1 200 OK\r\n";
    str_http_rsp += "Cache-Control: private\r\n";
    str_http_rsp += "Content-Type: application/json; charset=utf-8\r\n";
    str_http_rsp += "Content-Length: 30\r\n";
    str_http_rsp += "\r\n";
    str_http_rsp += "{\"State\":\"Success\",\"Msg\":\"OK\"}";

    http_request req;
    http_response rsp;
    http_context ctx;
    ctx.parser_http_context(str_http_req.c_str(), str_http_req.size(), req);
    ctx.parser_http_context(str_http_rsp.c_str(), str_http_rsp.size(), rsp);

    buffer buf;
    append_http_to_buf(req, buf);
    std::cout << buf.readable_start() << std::endl;

    buf.clear();
    append_http_to_buf(rsp, buf);
    std::cout << buf.readable_start() << std::endl;

    return 0;
}