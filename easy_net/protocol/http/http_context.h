/*
** http服务器主要时处理http请求的
*/
#ifndef __HTTP_CONTEXT_H
#define __HTTP_CONTEXT_H

#include "http_parser.h"
#include "http_request.h"

class http_context {
    enum parser_state
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        END
    };

public:
    const http_request& parser_http_context(const char* begin, const char* end);

private:
    //请求方法<br>URL<br>协议版本\r\n
    void _parser_request_line(const char* begin, const char* end);
    void _parser_headers(const char* begin, const char* end);

private:
    http_request* req_;
    parser_state curr_state_;
};
#endif