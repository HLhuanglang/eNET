#include "default_resolver.h"

#include <arpa/inet.h>
#include <netdb.h>

void EasyNet::DefaultResolver::resolve(const std::string& hostname, const Callback& callback) {
    char strIP[INET_ADDRSTRLEN] = {0};
    struct addrinfo* addr;
    int result = getaddrinfo(hostname.c_str(), NULL, NULL, &addr);
    if (result != 0) {
        printf("Error from getaddrinfo: %s\n", gai_strerror(result));
    }
    struct sockaddr_in* psa = (struct sockaddr_in*)addr->ai_addr;
    inet_ntop(AF_INET, &(psa->sin_addr), strIP, INET_ADDRSTRLEN);
    freeaddrinfo(addr);
    callback(InetAddress(strIP, 80));
}