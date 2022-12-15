#include "http_request.h"

void http_request::append_to_buffer(buffer* buf)
{
    //http响应结构
    //
    //[方法][空格][url][空格][版本信息]\r\n
    //请求头\r\n
    //\r\n
    //请求体

    buf->append(method_.c_str(), method_.size());
    buf->append(" ", 1);
    buf->append(url_.c_str(), url_.size());
    buf->append(" ", 1);
    buf->append(version_.c_str(), version_.size());
    buf->append("\r\n", 2);
    for (const auto& it : headers_) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf->append(str.c_str(), str.size());
    }
    buf->append("\r\n", 2);
    buf->append(body_.c_str(), body_.size());
}