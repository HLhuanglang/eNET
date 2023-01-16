#ifndef __EASYNET_UTIL_H
#define __EASYNET_UTIL_H

#include <regex>
#include <string>

namespace util {
bool check_ipv4(const std::string &ip);
bool check_port(int port);
} // namespace util
#endif