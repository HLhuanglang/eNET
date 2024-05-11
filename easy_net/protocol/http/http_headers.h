#ifndef __EASYNET_HTTP_HEADERS_H
#define __EASYNET_HTTP_HEADERS_H

#include <map>
namespace EasyNet {
class HttpHeaders {
 public:
    void SetHeader(const std::string &key, const std::string &val);
    void SetHeader(const char *key, const std::string &val);

 public:
    std::string SerializeToString();

 private:
    std::map<std::string, std::string> m_headers;
};
} // namespace EasyNet

#endif // !__EASYNET_HTTP_HEADERS_H
