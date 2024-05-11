#include "http_request.h"

using namespace EasyNet;

std::string HttpRequest::SerializeToString() {
    std::string res;
    res += m_method + " " + m_url + " " + m_version + "\r\n";
    res += m_headers.SerializeToString();
    res += "\r\n";
    res += m_body;
    return res;
}

std::string HttpRequest::SerializeToString() const {
    std::string res;
    res += m_method + " " + m_url + " " + m_version + "\r\n";
    res += m_headers.SerializeToString();
    res += "\r\n";
    res += m_body;
    return res;
}
