#include "http_response.h"
#include "buffer.h"
#include <string>

void http_response::default_init()
{

    version_                   = "HTTP/1.1";
    status_code_               = "200";
    status_code_msg_           = "Ok";
    headers_["Content-Length"] = "0";
}

void http_response::set_body(const char *body)
{
    body_                      = body;
    headers_["Content-Length"] = std::to_string(body_.size());
}

void http_response::append_http_to_buf(const http_response &rsp, buffer &buf)
{
    buf.append(rsp.version_.c_str(), rsp.version_.size());
    buf.append(" ", 1);
    buf.append(rsp.status_code_.c_str(), rsp.status_code_.size());
    buf.append(" ", 1);
    buf.append(rsp.status_code_msg_.c_str(), rsp.status_code_msg_.size());
    buf.append("\r\n", 2);
    for (const auto &it : rsp.headers_) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf.append(str.c_str(), str.size());
    }
    buf.append("\r\n", 2);
    buf.append(rsp.body_.c_str(), rsp.body_.size());
}