#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include <memory>
#include <string>

#include "http_headers.h"

namespace EasyNet {
class HttpRequest {
 public:
 public:
    std::string GetMethod() const { return m_method; }
    std::string GetUrl() const { return m_url; }
    void SetBody(const std::string &body) { m_body = body; }

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

using HttpRequestPtr = std::shared_ptr<HttpRequest>;
}  // namespace EasyNet

#endif