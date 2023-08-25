#include "http_request.h"

#include "buffer.h"

void http_request::append_http_to_buf(const http_request &req, buffer &buf) {
    buf.append(req.m_method.c_str(), req.m_method.size());
    buf.append(" ", 1);
    buf.append(req.m_url.c_str(), req.m_url.size());
    buf.append(" ", 1);
    buf.append(req.m_version.c_str(), req.m_version.size());
    buf.append("\r\n", 2);
    for (const auto &it : req.m_headers) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf.append(str.c_str(), str.size());
    }
    buf.append("\r\n", 2);
    buf.append(req.m_body.c_str(), req.m_body.size());
}