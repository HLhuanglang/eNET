#include "http_url.h"

#include <cstddef>
#include <vector>

#include "string_util.h"

// 解析url
// url格式：schema://host:port/path?query#fragment
bool EasyNet::HttpUrlParser::Parse() {
    if (m_url.empty()) {
        return false;
    }
    size_t begin = 0;
    size_t end = 0;
    size_t len = m_url.length();
    bool has_port = false;
    while (m_state != URL_STATE_END && end < len) {
        size_t fwd_step = 0;
        switch (m_state) {
            case URL_STATE_START:
                if (len < KProtocolMinLen) {
                    return false;
                }
                m_state = URL_STATE_PROTOCOL;
                break;
            case URL_STATE_PROTOCOL:
                if (m_url[end] == ':') {
                    url.protocol = m_url.substr(begin, end - begin);
                    begin = end + 3;  // skip "://"
                    fwd_step += 2;
                    m_state = URL_STATE_HOST;
                }
                break;
            case URL_STATE_HOST:
                if (end + 1 == len) {
                    url.host = m_url.substr(begin, end + 1 - begin);
                    m_state = URL_STATE_END;
                }
                if (m_url[end] == ':') {
                    url.host = m_url.substr(begin, end - begin);
                    begin = end + 1;  // skip ":"
                    m_state = URL_STATE_PORT;
                }
                if (m_url[end] == '/') {
                    url.host = m_url.substr(begin, end - begin);
                    begin = end;
                    m_state = URL_STATE_PATH;
                }
                break;
            case URL_STATE_PORT:
                if (m_url[end] == '/') {
                    url.port = m_url.substr(begin, end - begin);
                    begin = end;
                    m_state = URL_STATE_PATH;
                    has_port = true;
                }
                if (end + 1 == len) {
                    url.port = m_url.substr(begin, end + 1 - begin);
                    m_state = URL_STATE_END;
                    has_port = true;
                }
                break;
            case URL_STATE_PATH:
                if (m_url[end] == '?') {
                    url.path = m_url.substr(begin, end - begin);
                    begin = end + 1;
                    end = begin;
                    m_state = URL_STATE_QUERY;
                }
                if (end + 1 == len) {
                    url.path = m_url.substr(begin, end - begin);
                    m_state = URL_STATE_END;
                }
                break;
            case URL_STATE_QUERY:
                if (m_url[end] == '#') {
                    std::string query = m_url.substr(begin, end - begin);
                    std::vector<std::string> tokens;
                    split_string(query, tokens, "&");
                    for (auto token : tokens) {
                        auto pos = token.find("=");
                        if (pos == std::string::npos) {
                            break;
                        }
                    }
                    begin = end + 1;
                    m_state = URL_STATE_FRAGMENT;
                }
                if (end + 1 == len) {
                    std::string query = m_url.substr(begin, end - begin);
                    std::vector<std::string> tokens;
                    split_string(query, tokens, "&");
                    for (auto token : tokens) {
                        auto pos = token.find("=");
                        if (pos == std::string::npos) {
                            break;
                        }
                        auto key = token.substr(0, pos);
                        auto value = token.substr(pos + 1, token.length());
                        url.query[key] = value;
                    }
                    m_state = URL_STATE_END;
                }
                break;
            case URL_STATE_FRAGMENT:
                if (end + 1 == len) {
                    url.fragment = m_url.substr(begin, end + 1 - begin);
                    m_state = URL_STATE_END;
                }
                break;
        }
        end = end + 1 + fwd_step;
    }
    if (!has_port) {
        if (url.protocol == HTTP) {
            url.port = "80";
        }
        if (url.protocol == HTTPS) {
            url.port = "443";
        }
    }
    return true;
}