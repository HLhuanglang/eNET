#include "http_request.h"
#include "buffer.h"

void http_request::append_http_to_buf(const http_request &req, buffer &buf)
{
    buf.append(req.method_.c_str(), req.method_.size());
    buf.append(" ", 1);
    buf.append(req.url_.c_str(), req.url_.size());
    buf.append(" ", 1);
    buf.append(req.version_.c_str(), req.version_.size());
    buf.append("\r\n", 2);
    for (const auto &it : req.headers_) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf.append(str.c_str(), str.size());
    }
    buf.append("\r\n", 2);
    buf.append(req.body_.c_str(), req.body_.size());
}