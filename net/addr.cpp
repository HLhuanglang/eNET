#include "addr.h"
#include <arpa/inet.h> //for inet_pton
#include <cstring>

net_addr::net_addr(const std::string &ip, unsigned int port) {
    this->addr_.sin_family = AF_INET;
    this->addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &this->addr_.sin_addr);
}
