/*
** http服务器主要时处理http请求的
*/
#ifndef __HTTP_CONTEXT_H
#define __HTTP_CONTEXT_H

#include "http_parser.h"
#include "http_request.h"

class http_context {
public:
    http_context();
    size_t parser_http_context(const char* data, size_t len, http_request& req);

public:
    void _handle_header();

public:
    http_request* req_;
    std::string header_filed_;
    std::string header_value_;

private:
    static http_parser_settings parser_settings_;
    http_parser parser_;
};
#endif