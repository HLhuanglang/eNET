#ifndef __NET_H
#define __NET_H

#include <netinet/in.h>
#include <string>

class net_addr {
public:
    net_addr(const std::string &ip, unsigned int port);
    explicit net_addr(const struct sockaddr_in &addr) : addr_(addr) {}

public:
    std::string get_ip();

private:
    struct sockaddr_in addr_;
};

#endif