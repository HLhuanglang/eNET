/*
** http_parser是一个字符解析状态机，一个字符一个字符解析
** 只要消息是按照http协议格式来的,就能做解析
*/
#include "http_parser.h"
#include <iostream>
#include <string>

//所有的回调必须返回0,如果返回非0,就会停止解析

int on_msg_begin_cb(http_parser*)
{
    std::cout << "on_msg_begin_cb: begin parser!" << std::endl;
    return 0;
}
int on_url_cb(http_parser* parser, const char* at, size_t length)
{
    std::string url;
    url.assign(at, length);
    std::cout << "on_url_cb: url=" << url << std::endl;
    return 0;
}
int on_status_cb(http_parser*, const char* at, size_t length)
{
    // status针对于http响应的解析
    std::string status;
    status.assign(at, length);
    std::cout << "on_status_cb: status=" << status << std::endl;
    return 0;
}
int on_header_field_cb(http_parser*, const char* at, size_t length)
{
    // key-val中的key
    std::string key;
    key.assign(at, length);
    std::cout << "on_header_field_cb: key=" << key << std::endl;
    return 0;
}
int on_header_value_cb(http_parser*, const char* at, size_t length)
{
    // key-val中的val
    std::string val;
    val.assign(at, length);
    std::cout << "on_header_value_cb: val=" << val << std::endl;
    return 0;
}
int on_headers_complete_cb(http_parser*)
{
    //所有的头部解析完成了
    std::cout << "on_headers_complete_cb: headers parserd" << std::endl;
    return 0;
}
int on_body_cb(http_parser*, const char* at, size_t length)
{
    //实际使用时,并不直接一次性全部读取了，如果body很大,执行一次拷贝动作会占用很多的内存...
    std::string body;
    body.assign(at, length);
    std::cout << "on_body_cb: body" << body << std::endl;
    return 0;
}
int on_msg_complete_cb(http_parser*)
{
    //整个http协议解析完成
    std::cout << "on_msg_complete_cb: end msg parser!" << std::endl;
    return 0;
}

// int on_chunk_header_cb(http_parser*) {}
// int on_chunk_complete_cb(http_parser*) {}

int main(int argc, char* argv[])
{
    int ret_bytes = 0;

    //-=-------------------http-req----------------------------

    // 1,待解析的请求报文
    std::string str_http_req = "";
    str_http_req += "POST /uploaddata HTTP/1.1\r\n";
    str_http_req += "Host: 127.0.0.1:8080\r\n";
    str_http_req += "Accept: */*\r\n";
    str_http_req += "Content-Length: 70\r\n";
    str_http_req += "Content-Type: application/json\r\n";
    str_http_req += "\r\n";
    str_http_req += "{\"DeviceID\":\"12345678\",\"LowPressure\":80,\"HighPressure\":120,\"Pulse\":90}";

    // 2,初始化解析器
    http_parser parser_req;
    http_parser_init(&parser_req, http_parser_type::HTTP_REQUEST);
    http_parser_settings settings_req;
    http_parser_settings_init(&settings_req);
    settings_req.on_message_begin    = on_msg_begin_cb;
    settings_req.on_url              = on_url_cb;
    settings_req.on_status           = on_status_cb;
    settings_req.on_header_field     = on_header_field_cb;
    settings_req.on_header_value     = on_header_value_cb;
    settings_req.on_headers_complete = on_headers_complete_cb;
    settings_req.on_body             = on_body_cb;
    settings_req.on_message_complete = on_msg_complete_cb;
    // settings_req.on_chunk_header = on_chunk_header_cb;
    // settings_req.on_chunk_complete = on_chunk_complete_cb;

    // 3,执行解析器
    // ----http_parser中method记录了请求类型
    // ----htp_parser中status_code记录了返回码
    ret_bytes = http_parser_execute(&parser_req, &settings_req, str_http_req.c_str(), str_http_req.size());
    std::cout << "ret_bytes=" << ret_bytes << " size=" << str_http_req.size() << std::endl;

    //-=-------------------http-res----------------------------
    std::string str_http_res = "";
    str_http_res += "HTTP/1.1 200 OK\r\n";
    str_http_res += "Cache-Control: private\r\n";
    str_http_res += "Content-Type: application/json; charset=utf-8\r\n";
    str_http_res += "Content-Length: 30\r\n";
    str_http_res += "\r\n";
    str_http_res += "{\"State\":\"Success\",\"Msg\":\"OK\"}";

    http_parser parser_res;
    http_parser_init(&parser_res, http_parser_type::HTTP_RESPONSE);
    http_parser_settings settings_res;
    http_parser_settings_init(&settings_res);
    settings_res.on_message_begin    = on_msg_begin_cb;
    settings_res.on_url              = on_url_cb;
    settings_res.on_status           = on_status_cb;
    settings_res.on_header_field     = on_header_field_cb;
    settings_res.on_header_value     = on_header_value_cb;
    settings_res.on_headers_complete = on_headers_complete_cb;
    settings_res.on_body             = on_body_cb;
    settings_res.on_message_complete = on_msg_complete_cb;
    // settings_res.on_chunk_header = on_chunk_header_cb;
    // settings_res.on_chunk_complete = on_chunk_complete_cb;

    // 3,执行解析器
    // ----http_parser中method记录了请求类型
    // ----htp_parser中status_code记录了返回码
    ret_bytes = http_parser_execute(&parser_res, &settings_res, str_http_res.c_str(), str_http_res.size());
    std::cout << "ret_bytes=" << ret_bytes << " size=" << str_http_res.size() << std::endl;

    return 0;
}