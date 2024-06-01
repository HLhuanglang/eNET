#ifndef __EASYNET_DEFAULT_RESOLVER_H
#define __EASYNET_DEFAULT_RESOLVER_H

#include "resolver.h"
namespace EasyNet {
class DefaultResolver : public DNSResolver {
 public:
    DefaultResolver();
    void resolve(const std::string& hostname, const Callback& callback) override;
    void resolve(const std::string& hostname, const ResolverResultsCallback& callback) override;
};
}  // namespace EasyNet

#endif  // !__EASYNET_DEFAULT_RESOLVER_H
