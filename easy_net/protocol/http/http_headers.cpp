#include "http_headers.h"

using namespace EasyNet;

std::string HttpHeaders::SerializeToString() {
    std::string res;
    for (const auto &header : m_headers) {
        res += header.first + ": " + header.second + "\r\n";
    }
    return res;
}

void HttpHeaders::SetHeader(const std::string &key, const std::string &val) {
    m_headers[key] = val;
}

void HttpHeaders::SetHeader(const char *key, const std::string &val) {
    m_headers[key] = val;
}