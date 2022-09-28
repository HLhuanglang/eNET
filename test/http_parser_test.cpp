#include "easy_net.h"
#include "http_context.h"

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

    http_request req;
    http_context ctx;
    ctx.parser_http_context(str_http_req.c_str(), str_http_req.size(), req);
    return 0;
}