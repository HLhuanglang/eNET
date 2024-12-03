#include "inet_addr.h"

#include <cstdint>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "log.h"

using namespace EasyNet;

InetAddress::InetAddress(const char *ip, uint16_t port, bool ipv6) : m_ipv6(ipv6) {
    if (ipv6) {
        memset(&m_addr6, 0, sizeof m_addr6);
        m_addr6.sin6_family = AF_INET6;
        m_addr6.sin6_port = htons(port);
        if (::inet_pton(AF_INET6, ip, &m_addr6.sin6_addr) <= 0) {
            LOG_ERROR("IPaddress Error");
        }
    } else {
        memset(&m_addr, 0, sizeof m_addr);
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        if (::inet_pton(AF_INET, ip, &m_addr.sin_addr) <= 0) {
            LOG_ERROR("IPaddress Error");
        }
    }
}

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6) : m_ipv6(ipv6) {
    if (ipv6) {
        memset(&m_addr6, 0, sizeof m_addr6);
        m_addr6.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        m_addr6.sin6_addr = ip;
        m_addr6.sin6_port = htons(port);
    } else {
        memset(&m_addr, 0, sizeof m_addr);
        m_addr.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
        m_addr.sin_addr.s_addr = htonl(ip);
        m_addr.sin_port = htons(port);
    }
}

std::string InetAddress::SerializationToIpPort() const {
    char buf[64] = "";
    size_t size = sizeof(buf);
    uint16_t port;

    if (m_ipv6) {
        assert(size >= INET6_ADDRSTRLEN);
        port = ntohs(m_addr6.sin6_port);
        ::inet_ntop(AF_INET6, &m_addr6.sin6_addr, buf, static_cast<socklen_t>(size));
    } else {
        assert(size >= INET_ADDRSTRLEN);
        port = ntohs(m_addr.sin_port);
        ::inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t>(size));
    }
    size_t end = ::strlen(buf);
    assert(size > end);
    snprintf(buf + end, size - end, ":%u", port);

    return buf;
}

std::string InetAddress::SerializationToIP() const {
    char buf[64] = "";
    size_t size = sizeof(buf);
    if (m_ipv6) {
        assert(size >= INET6_ADDRSTRLEN);
        ::inet_ntop(AF_INET6, &m_addr6.sin6_addr, buf, static_cast<socklen_t>(size));
    } else {
        assert(size >= INET_ADDRSTRLEN);
        ::inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t>(size));
    }
    return buf;
}

std::string InetAddress::SerializationToPort() const {
    char buf[64] = "";
    size_t size = sizeof(buf);
    uint16_t port;
    if (m_ipv6) {
        assert(size >= INET6_ADDRSTRLEN);
        port = ntohs(m_addr6.sin6_port);
    } else {
        assert(size >= INET_ADDRSTRLEN);
        port = ntohs(m_addr.sin_port);
    }
    snprintf(buf, size, "%u", port);
    return buf;
}

void InetAddress::setPort(uint16_t port) {
    if (m_ipv6) {
        m_addr6.sin6_port = htons(port);
    } else {
        m_addr.sin_port = htons(port);
    }
}