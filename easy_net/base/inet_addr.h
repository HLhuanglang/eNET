#ifndef __EASYNET_INET_ADDR_H_
#define __EASYNET_INET_ADDR_H_

#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

class inet_addr {
 public:
    enum inet_family_e {
        IPV4,
        IPV6
    };

 public:
    // 如果是ipv6
    //      1,loopback_only为true: ip为"::"
    //      2,loopback_only为false: ip为"::1"
    // 如果是ipv4
    //      1,loopback_only为true: ip为"127.0.0.1"
    //      2,loopback_only为false: ip为"0.0.0.0"
    // inet_addr(bool ipv6 = false, bool loopback_only = false, uint16_t port = 0);

    // 如果是ipv6，ip的格式为"fe80::5054:ff:fedd:d80d"
    // 如果是ipv4，ip的格式为"192.168.0.1“
    inet_addr(inet_family_e type, const char *ip = "", uint16_t port = 0);

    explicit inet_addr(const struct sockaddr_in &addr) : m_addr(addr) {}
    explicit inet_addr(const struct sockaddr_in6 &addr) : m_addr6(addr) {}

 public:
    inet_family_e get_family() const;
    std::string get_ip() const;
    uint16_t get_port() const;

 private:
    union {
        struct sockaddr_in m_addr;
        struct sockaddr_in6 m_addr6;
    };
};

#endif // !__EASYNET_INET_ADDR_H_
