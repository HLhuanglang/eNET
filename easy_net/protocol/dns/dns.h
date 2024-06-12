#ifndef __EASYNET_DNS_H
#define __EASYNET_DNS_H

#include <functional>
#include <memory>
#include <vector>

#include "inet_addr.h"
#include "non_copyable.h"

namespace EasyNet {

const constexpr int DNS_PORT = 53;  // dns服务器的端口号,一般默认都是53

class DNSResolver : public NonCopyable {
    using Callback = std::function<void(const std::vector<InetAddress>&)>;

 public:
    static std::shared_ptr<DNSResolver> CreateResolver();
    void resolve(const std::string& hostname, const Callback& callback);
    void resolve(const std::string& hostname, const std::string& nameserver, const Callback& callback);
};

using DNSResolverPtr = std::shared_ptr<DNSResolver>;

}  // namespace EasyNet

#endif  // !__EASYNET_DNS_H
