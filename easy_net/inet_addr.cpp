#include "inet_addr.h"
#include <arpa/inet.h>
#include <cstdint>
#include <endian.h>
#include <string>
#include <strings.h>

#include "log.h"

// inet_addr::inet_addr(bool ipv6, bool loopback_only, uint16_t port) {
//     if (ipv6) {
//         bzero(&m_addr6, sizeof(m_addr6));
//         m_addr6.sin6_family = AF_INET6;
//         m_addr6.sin6_addr = loopback_only ? in6addr_loopback : in6addr_any;
//         m_addr6.sin6_port = htobe16(port);
//     } else {
//         bzero(&m_addr, sizeof(m_addr));
//         m_addr.sin_family = AF_INET;
//         in_addr_t ip = loopback_only ? INADDR_LOOPBACK : INADDR_ANY;
//         m_addr.sin_addr.s_addr = htobe32(ip);
//         m_addr.sin_port = htobe16(port);
//     }
// }

inet_addr::inet_addr(bool ipv6, const char *ip, uint16_t port) {
    if (ipv6) {
        bzero(&m_addr6, sizeof m_addr6);
        m_addr6.sin6_family = AF_INET6;
        m_addr6.sin6_port = htobe16(port);
        if (::inet_pton(AF_INET6, ip, &m_addr6.sin6_addr) <= 0) {
            LOG_FATAL("IPaddress Error");
        }
    } else {
        bzero(&m_addr, sizeof m_addr);
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htobe16(port);
        if (::inet_pton(AF_INET, ip, &m_addr.sin_addr) <= 0) {
            LOG_FATAL("IPaddress Error");
        }
    }
}

inet_addr::inet_family_e inet_addr::get_family() const {
    return m_addr.sin_family == AF_INET ? IPV4 : IPV6;
}

std::string inet_addr::get_ip() const {
    char buf[64] = "";
    if (m_addr.sin_family == AF_INET) {
        ::inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof buf);
    } else if (m_addr.sin_family == AF_INET6) {
        ::inet_ntop(AF_INET6, &m_addr6.sin6_addr, buf, sizeof buf);
    }
    return buf;
}

uint16_t inet_addr::get_port() const {
    uint16_t port = 0;
    if (m_addr.sin_family == AF_INET) {
        port = be16toh(m_addr.sin_port);
    } else if (m_addr.sin_family == AF_INET6) {
        port = be16toh(m_addr6.sin6_port);
    }
    return port;
}