/*
** req一般是客户端发送过来的请求,这个时候,我们的服务器需要解析这坨http消息
** 可以采用http_parser进行解析,将解析完的数据构造成http_request对象即可.
*/
#ifndef __EASYNET_HTTP_REQUEST_H
#define __EASYNET_HTTP_REQUEST_H

#include <map>
#include <string>

#include "buffer.h"

class http_request {
 public:
    http_request() = default;

 public:
    static void append_http_to_buf(const http_request &req, buffer &buf);

 public:
    std::string m_method;
    std::string m_url;
    std::string m_version;
    std::map<std::string, std::string> m_headers; // key-val
    std::string m_body;
};

#endif