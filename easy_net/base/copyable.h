#ifndef __EASYNET_COPYABLE_H
#define __EASYNET_COPYABLE_H

namespace EasyNet {
// 允许拷贝
class Copyable {
 public:
    Copyable() = default;
    ~Copyable() = default;
};
}  // namespace EasyNet

#endif