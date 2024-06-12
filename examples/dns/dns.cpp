#include "dns.h"

#include <string>

#include "log.h"

using namespace EasyNet;

int main() {
    LogInit(level::debug);
    auto dns = DNSResolver::CreateResolver();
    dns->resolve("www.baidu.com", [](const std::vector<InetAddress>& addr) {
        for (auto& ip : addr) {
            LOG_DEBUG("ip: {}", ip.SerializationToIpPort());
        }
    });
    return 0;
}