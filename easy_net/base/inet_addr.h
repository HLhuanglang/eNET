#ifndef __EASYNET_INET_ADDR_H_
#define __EASYNET_INET_ADDR_H_

#ifdef _WIN32
#    include <WinSock2.h>
#    include <ws2tcpip.h>
using sa_family_t = unsigned short;
using in_addr_t = uint32_t;
using uint16_t = unsigned short;
#else
#    include <arpa/inet.h>
#    include <netinet/in.h>
#    include <sys/socket.h>
#endif

#include <string>

/*
see this post about endian:
https://github.com/HLhuanglang/EasyNet/blob/main/docs/md/generic/%E4%B8%BB%E6%9C%BA%E5%AD%97%E8%8A%82%E5%BA%8F%E5%92%8C%E7%BD%91%E7%BB%9C%E5%AD%97%E8%8A%82%E5%BA%8F.md
*/
namespace EasyNet {
class InetAddress {
 public:
    InetAddress(const char *ip, uint16_t port, bool ipv6 = false);
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
    explicit InetAddress(const struct sockaddr_in &addr)
        : m_addr(addr) {}
    explicit InetAddress(const struct sockaddr_in6 &addr)
        : m_addr6(addr), m_ipv6(true) {}

 public:
    sa_family_t family() const {
        if (m_ipv6) {
            return m_addr6.sin6_family;
        } else {
            return m_addr.sin_family;
        }
    }
    std::string SerializationToIpPort() const;
    std::string SerializationToIP() const;
    std::string SerializationToPort() const;

    const struct sockaddr *GetAddr() const {
        if (m_ipv6) {
            return (sockaddr *)(&m_addr6);
        } else {
            return (sockaddr *)(&m_addr);
        }
    }

    size_t GetAddrSize() const {
        if (m_ipv6) {
            return sizeof(sockaddr_in6);
        } else {
            return sizeof(sockaddr_in);
        }
    }

    void setSockAddrInet6(const struct sockaddr_in6 &addr6) { m_addr6 = addr6; }
    void setSockAddrInet(const struct sockaddr_in &addr) { m_addr = addr; }

 private:
    /*
    struct sockaddr {
        sa_family_t    sa_family;      // 套接字域，例如AF_INET、AF_INET6等
        char           sa_data[14];    // 套接字地址数据
    };

    struct sockaddr_in {
        sa_family_t    sin_family;     // 套接字域，通常为AF_INET
        in_port_t      sin_port;       // 端口号，网络字节序
        struct in_addr sin_addr;       // IPv4地址
        unsigned char  sin_zero[8];    // 填充字节，保持结构体大小一致
    };

    struct sockaddr_in6 {
        sa_family_t     sin6_family;   // 套接字域，通常为AF_INET6
        in_port_t       sin6_port;     // 端口号，网络字节序
        uint32_t        sin6_flowinfo; // 流信息，通常为0
        struct in6_addr sin6_addr;     // IPv6地址
        uint32_t        sin6_scope_id; // 作用域ID，用于链路本地地址
    };
    */
    union {
        struct sockaddr_in m_addr;
        struct sockaddr_in6 m_addr6;
    };
    bool m_ipv6 = false;
};
}  // namespace EasyNet

#endif  // !__EASYNET_INET_ADDR_H_
