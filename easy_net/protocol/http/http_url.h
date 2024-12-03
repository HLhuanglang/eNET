#ifndef __EASYNET_URL_H
#define __EASYNET_URL_H

#include <map>
#include <string>

namespace EasyNet {

constexpr int KProtocolMinLen = 7;  // http://
const std::string HTTP = "http";
const std::string HTTPS = "https";

typedef struct url_data {
    std::string protocol;                      // http, https
    std::string host;                          // www.hlllz.cn
    std::string port;                          // 80, 443
    std::string path;                          // /v1/api
    std::map<std::string, std::string> query;  // ?name=xxx&age=18
    std::string fragment;                      // #fragment
} url_data_t;

typedef enum url_state {
    URL_STATE_START = 0,
    URL_STATE_PROTOCOL,
    URL_STATE_HOST,
    URL_STATE_PORT,
    URL_STATE_PATH,
    URL_STATE_QUERY,
    URL_STATE_FRAGMENT,
    URL_STATE_END,
} url_state_t;

class HttpUrlParser {
 public:
    HttpUrlParser(const std::string &url) : m_url(url) {}
    bool Parse();

 public:
    url_data_t url;

 private:
    std::string m_url;
    url_state_t m_state = URL_STATE_START;
};
}  // namespace EasyNet

#endif  // !__EASYNET_URL_H
