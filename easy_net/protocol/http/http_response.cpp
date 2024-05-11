#include "http_response.h"
#include <string>

using namespace EasyNet;

std::string HttpResponse::SerializeToString() {
    // 处理头
    m_headers.SetHeader("Content-Length", std::to_string(m_body.size()));
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
    m_headers.SetHeader("Connection", "close");
    m_headers.SetHeader("Content-Type", "text/html");

    std::string res;
    res += m_version + " " + m_status_code + " " + m_status_code_msg + "\r\n";
    res += m_headers.SerializeToString();
    res += "\r\n";
    res += m_body;
    return res;
}
