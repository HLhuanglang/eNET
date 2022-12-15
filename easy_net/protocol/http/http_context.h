/*
** http服务器主要时处理http请求的
*/
#ifndef __HTTP_CONTEXT_H
#define __HTTP_CONTEXT_H

#include "http_parser.h"
#include "http_request.h"
#include "http_response.h"

enum http_type_t
{
    HTTP_REQ, //请求
    HTTP_RSP, //响应
};

class http_context {
public:
    size_t parser_http_context(const char* data, size_t len, http_request& req);
    size_t parser_http_context(const char* data, size_t len, http_response& rsp);

public:
    void _handle_header();

public:
    http_type_t type_;
    http_request* req_;
    http_response* rsp_;
    std::string header_filed_;
    std::string header_value_;

private:
    static http_parser_settings parser_settings_;
    http_parser parser_;
};
#endif