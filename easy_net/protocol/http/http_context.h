/*
** http服务器主要时处理http请求的
*/
#ifndef __EASYNET_HTTP_CONTEXT_H
#define __EASYNET_HTTP_CONTEXT_H

#include "http_parser.h"
#include "http_request.h"
#include "http_response.h"

enum http_type_t {
    HTTP_REQ, // 请求
    HTTP_RSP, // 响应
};

class http_context {
 public:
    size_t parser_http_context(const char *data, size_t len, http_request &req);
    size_t parser_http_context(const char *data, size_t len, http_response &rsp);

    bool is_http_complete() const { return m_finish_one_parse; }

 public:
    void handle_header();

 public:
    http_type_t m_type;
    http_request *m_req = nullptr;
    http_response *m_rsp = nullptr;
    std::string m_header_filed;
    std::string m_header_value;
    bool m_finish_one_parse = false;

 private:
    static http_parser_settings s_parser_settings;
    http_parser m_parser;
};
#endif