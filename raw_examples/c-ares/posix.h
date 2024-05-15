#ifndef __EASYNET_POSIX_H
#define __EASYNET_POSIX_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <cstdio>
#include <string>

inline void posix(std::string domain = "hlllz.cn") {
    char strIP[INET_ADDRSTRLEN] = {0};
    struct addrinfo* addr;
    int result = getaddrinfo(domain.c_str(), NULL, NULL, &addr);
    if (result != 0) {
        printf("Error from getaddrinfo: %s\n", gai_strerror(result));
    }
    struct sockaddr_in* psa = (struct sockaddr_in*)addr->ai_addr;
    inet_ntop(AF_INET, &(psa->sin_addr), strIP, INET_ADDRSTRLEN);
    freeaddrinfo(addr);
    printf("Found address: %s\n", strIP);
}
#endif  // !__EASYNET_POSIX_H
