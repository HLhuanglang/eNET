#include <iostream>

#include "easy_net/util.h"

int main() {
    std::string ip = "127.0.0.1";
    bool ret = util::check_ipv4(ip);
    if (ret) {
        std::cout << "is valid ip!" << std::endl;
    } else {
        std::cout << "is invalid ip!" << std::endl;
    }
    return 0;
}