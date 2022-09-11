#ifndef __UTIL_H
#define __UTIL_H

#include <regex>
#include <string>

namespace util {

bool check_ipv4(const std::string& ip) {
    std::regex pattern("((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    std::smatch res;
    if (regex_match(ip, res, pattern)) {
        return true;
    }
    return false;
}

bool check_port(int port) {
    return port < 65536 ? true : false;
}

} // namespace util
#endif