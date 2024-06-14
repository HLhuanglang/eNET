#include "http_response.h"

#include <string>

using namespace EasyNet;

std::string HttpResponse::SerializeToString() {
    // 处理头
    m_headers.SetHeader("Content-Length", std::to_string(m_body.size()));
    // 客户端或服务器发现对方一段时间没有活动，就可以主动关闭连接
    // 不过，规范的做法是，客户端在最后一个请求时，发送 Connection: close，明确要求服务器关闭 TCP 连接。
    // http1.1默认是keep-alive
    m_headers.SetHeader("Connection", "keep-alive");
    m_headers.SetHeader("Content-Type", "text/html");

    std::string res;
    res += m_version + " " + m_status_code + " " + m_status_code_msg + "\r\n";
    res += m_headers.SerializeToString();
    res += "\r\n";
    res += m_body;
    return res;
}

std::string HttpResponse::SerializeToString() const {
    // 处理头
    m_headers.SetHeader("Content-Length", std::to_string(m_body.size()));
    m_headers.SetHeader("Connection", "keep-alive");  // http1.1默认是keep-alive
    m_headers.SetHeader("Content-Type", "text/html");

    std::string res;
    res += m_version + " " + m_status_code + " " + m_status_code_msg + "\r\n";
    res += m_headers.SerializeToString();
    res += "\r\n";
    res += m_body;
    return res;
}
