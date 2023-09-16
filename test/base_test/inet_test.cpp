#include "easy_net/inet_addr.h"
#include <cstddef>
#include <iostream>

int main() {
    inet_addr addr_v6(inet_addr::IPV6, "fe80::5054:ff:fedd:d80d", 12345);
    if (addr_v6.get_family() == inet_addr::IPV4) {
        std::cout << "ipv4" << std::endl;
    } else if (addr_v6.get_family() == inet_addr::IPV6) {
        std::cout << "ipv6" << std::endl;
    }

    std::cout << addr_v6.get_ip() << std::endl;
    std::cout << addr_v6.get_port() << std::endl;

    return 0;
}