#ifndef __EASYNET_HTTP_ROUTER_H
#define __EASYNET_HTTP_ROUTER_H

#include <map>
#include <string>

#include "http_def.h"
#include "http_request.h"
#include "http_response.h"
#include "log.h"

namespace EasyNet {
class HttpServer;
class HttpRouter {
    friend class HttpServer;

 public:
    // 支持通配符
    void GET(const std::string &cmd, const HttpCallBack &cb) {
        m_get_router[cmd] = cb;
    }

    // 支持通配符
    void POST(const std::string &cmd, const HttpCallBack &cb) {
        m_post_router[cmd] = cb;
    }

 private:
    void Routing(const HttpRequest &req, HttpResponse &resp) {
        auto method = req.GetMethod();
        auto cmd = req.GetUrl();
        LOG_DEBUG("Routing: {} {}", method, cmd);
        if (method == "GET") {
            auto it = m_get_router.find(cmd);
            if (it != m_get_router.end()) {
                it->second(req, resp);
            }
        } else if (method == "POST") {
            auto it = m_post_router.find(cmd);
            if (it != m_post_router.end()) {
                it->second(req, resp);
            }
        }
    }

 private:
    std::map<std::string, HttpCallBack> m_get_router;
    std::map<std::string, HttpCallBack> m_post_router;
};
}  // namespace EasyNet

#endif  // !__EASYNET_HTTP_ROUTER_H
