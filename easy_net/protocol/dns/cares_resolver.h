#ifndef __EASYNET_CARES_RESOLVER_H
#define __EASYNET_CARES_RESOLVER_H

#include "resolver.h"

namespace EasyNet {
class CAresResolver : public DNSResolver {
 public:
    CAresResolver();
    void resolve(const std::string& hostname, const Callback& callback) override;
    void resolve(const std::string& hostname, const ResolverResultsCallback& callback) override;
};
}  // namespace EasyNet

#endif  // !__EASYNET_CARES_RESOLVER_H
