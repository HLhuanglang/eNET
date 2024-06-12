#include "dns.h"

#include <arpa/inet.h>
#include <netdb.h>

#include <cstring>

#include "log.h"

using namespace EasyNet;

DNSResolverPtr DNSResolver::CreateResolver() {
    return std::make_shared<DNSResolver>();
}

/*
  一般机器上可以使用nslookup和dig来查询域名对应的Ip地址，在ubuntu上一般是请求named服务：
    - https://www.cnblogs.com/heyongshen/p/17794738.html
  使用getaddrinfo和getnameinfo函数来进行域名解析，比较慢：
    - https://jameshfisher.com/2018/02/03/what-does-getaddrinfo-do/
    - http://blog.gerryyang.com/tcp/ip/2022/05/12/c-ares-in-action.html
 */
void DNSResolver::resolve(const std::string& hostname, const Callback& callback) {
    std::vector<InetAddress> ips;
    struct addrinfo hints, *servinfo, *p;
    // 设置hints结构体以获取IPv4和IPv6地址
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // 不指定地址族，允许IPv4和IPv6
    hints.ai_socktype = SOCK_STREAM;  // 指定套接字类型为流式套接字
    int result = getaddrinfo(hostname.c_str(), nullptr, &hints, &servinfo);
    if (result != 0) {
        LOG_ERROR("getaddrinfo: {}", gai_strerror(result));
        goto end;
    }

    // 遍历所有返回的地址信息
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if (p->ai_family == AF_INET) {  // IPv4地址
            struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
            char ipv4_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipv4_str, INET_ADDRSTRLEN);
            ips.emplace_back(ipv4_str, ntohs(ipv4->sin_port));
        } else if (p->ai_family == AF_INET6) {  // IPv6地址
            struct sockaddr_in6* ipv6 = reinterpret_cast<struct sockaddr_in6*>(p->ai_addr);
            char ipv6_str[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6_str, INET6_ADDRSTRLEN);
            ips.emplace_back(ipv6_str, ntohs(ipv6->sin6_port), true);
        }
    }

end:
    callback(ips);
    freeaddrinfo(servinfo);
}

void DNSResolver::resolve(const std::string& hostname, const std::string& nameserver, const Callback& callback) {
    // TODO: 自己实现dns协议请求
}