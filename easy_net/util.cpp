#include "util.h"

bool util::check_ipv4(const std::string &ip) {
    std::regex pattern("((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    std::smatch res;
    return regex_match(ip, res, pattern);
}

bool util::check_port(int port) {
    return port < 65536;
}