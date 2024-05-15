
#include "cares.h"
#include "posix.h"
#include "time_cost.h"

// 使用cares库实现域名解析
void resolve1() {
    ScopedTimer timer("resolve1");
    char strIP[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in sa = {};
    std::string domain = "hlllz.cn";
    int timeout_ms = 1000;
    DNSResolver dr;
    if (dr) {
        auto ret = dr.lookup(domain, timeout_ms, &sa.sin_addr.s_addr, sizeof(sa.sin_addr.s_addr));
        if (0 != ret) {
            printf("resolve ret(%d) err(%s)\n", ret, dr.error_info().c_str());
            return;
        }
        inet_ntop(AF_INET, &(sa.sin_addr), strIP, INET_ADDRSTRLEN);
        printf("strIP=%s\n", strIP);
    } else {
        printf("dns_resolver_t init err(%s)\n", dr.error_info().c_str());
        return;
    }
}

// 使用POSIX接口实现域名解析
void resolve2() {
    ScopedTimer timer("resolve2");
    posix("hlllz.cn");
}

int main() {
    resolve1();
    resolve2();
}