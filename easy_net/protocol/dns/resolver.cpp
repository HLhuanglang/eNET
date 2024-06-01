#include "resolver.h"

#ifdef HAS_CARES
#    include "cares_resolver.h"
#else
#    include "default_resolver.h"
#endif

using namespace EasyNet;

std::shared_ptr<DNSResolver> DNSResolver::CreateResolver() {
#ifdef HAS_CARES
    return std::make_shared<CAresResolver>();
#else
    return std::make_shared<DefaultResolver>();
#endif
}