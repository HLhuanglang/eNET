#ifndef __HTTP_CONTEXT_H
#define __HTTP_CONTEXT_H

#include <cstddef>

#include "http_parser.h"
#include "http_request.h"
#include "http_response.h"

namespace EasyNet {
class HttpContext {
 public:
    class Status {
     public:
        enum ParseSatatus {
            PARSE_OK = 0,  // 解析完成
            PARSE_ERROR,   // 解析出错
        };

     public:
        Status(ParseSatatus status, size_t parsed_len)
            : m_status(status), m_parsed_len(parsed_len) {
        }
        bool ParseComplete() const {
            return m_status == PARSE_OK;
        }

        size_t GetParsedLen() const {
            return m_parsed_len;
        }

     private:
        ParseSatatus m_status;
        size_t m_parsed_len;
    };

 public:
    HttpContext(http_parser_type type);
    Status Parse(const char *data, size_t len, HttpRequest &req);
    Status Parse(const char *data, size_t len, HttpResponse &rsp);

 public:
    http_parser_type m_type;
    HttpRequest *m_req;
    HttpResponse *m_rsp;
    std::string m_header_filed;
    std::string m_header_value;

 private:
    static http_parser_settings s_parser_settings;
    http_parser m_parser;
};
}  // namespace EasyNet

#endif