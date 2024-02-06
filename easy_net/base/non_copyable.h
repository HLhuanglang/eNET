#ifndef __EASYNET_NON_COPYABLE_H
#define __EASYNET_NON_COPYABLE_H

#include <memory>
#include <utility>

namespace EasyNet {
/// @brief 屏蔽拷贝构造和赋值函数
class NonCopyable {
 protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

 public:
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
};

/// @brief 创建unique_ptr
/// @note cpp11只有make_shared，缺少make_unique cpp14才有
template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace EasyNet

#endif