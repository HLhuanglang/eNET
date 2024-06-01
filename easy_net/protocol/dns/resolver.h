#ifndef __EASYNET_RESOLVER_H
#define __EASYNET_RESOLVER_H

#include <functional>
#include <memory>
#include <vector>

#include "inet_addr.h"

namespace EasyNet {
class DNSResolver {
 protected:
    using Callback = std::function<void(const InetAddress&)>;
    using ResolverResultsCallback = std::function<void(const std::vector<InetAddress>&)>;

 public:
    DNSResolver() = default;
    virtual ~DNSResolver() = default;
    virtual void resolve(const std::string& hostname, const Callback& callback) = 0;
    virtual void resolve(const std::string& hostname, const ResolverResultsCallback& callback) = 0;

 public:
    static std::shared_ptr<DNSResolver> CreateResolver();
};
}  // namespace EasyNet

#endif  // !__EASYNET_RESOLVER_H
