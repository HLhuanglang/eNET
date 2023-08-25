#include "http_response.h"
#include <string>

#include "buffer.h"

void http_response::default_init() {
    m_version = "HTTP/1.1";
    m_status_code = "200";
    m_status_code_msg = "Ok";
    m_headers["Content-Length"] = "0";
}

void http_response::set_body(const char *body) {
    m_body = body;
    m_headers["Content-Length"] = std::to_string(m_body.size());
}

void http_response::append_http_to_buf(const http_response &rsp, buffer &buf) {
    buf.append(rsp.m_version.c_str(), rsp.m_version.size());
    buf.append(" ", 1);
    buf.append(rsp.m_status_code.c_str(), rsp.m_status_code.size());
    buf.append(" ", 1);
    buf.append(rsp.m_status_code_msg.c_str(), rsp.m_status_code_msg.size());
    buf.append("\r\n", 2);
    for (const auto &it : rsp.m_headers) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf.append(str.c_str(), str.size());
    }
    buf.append("\r\n", 2);
    buf.append(rsp.m_body.c_str(), rsp.m_body.size());
}