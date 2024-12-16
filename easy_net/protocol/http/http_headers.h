#ifndef __EASYNET_HTTP_HEADERS_H
#define __EASYNET_HTTP_HEADERS_H

#include <map>
#include <string>

namespace EasyNet {
class HttpHeaders {
 public:
    void SetHeader(const std::string &key, const std::string &val);
    void SetHeader(const char *key, const std::string &val);
    void SetHeader(std::map<std::string, std::string> headers);

 public:
    std::string SerializeToString();

 private:
    std::map<std::string, std::string> m_headers;
};
}  // namespace EasyNet

#endif  // !__EASYNET_HTTP_HEADERS_H
