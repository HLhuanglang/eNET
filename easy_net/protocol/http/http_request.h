#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include <string>

#include "http_headers.h"

namespace EasyNet {
class HttpRequest {
 public:
    std::string GetMethod() const { return m_method; }
    std::string GetUrl() const { return m_url; }

 public:
    std::string SerializeToString();
    std::string SerializeToString() const;

 public:
    std::string m_method;
    std::string m_url;
    std::string m_version{"HTTP/1.1"};
    mutable HttpHeaders m_headers;  // key-val
    std::string m_body;
};
}  // namespace EasyNet

#endif