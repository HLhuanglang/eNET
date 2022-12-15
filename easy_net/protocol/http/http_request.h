/*
** req一般是客户端发送过来的请求,这个时候,我们的服务器需要解析这坨http消息
** 可以采用http_parser进行解析,将解析完的数据构造成http_request对象即可.
*/
#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include <map>
#include <string>

#include "buffer.h"

class http_request {
public:
    void append_to_buffer(buffer* buf);

public:
    std::string method_;
    std::string url_;
    std::string version_;
    std::map<std::string, std::string> headers_; // key-val
    std::string body_;
};

#endif