#ifndef __EASYNET_NON_COPYABLE_H
#define __EASYNET_NON_COPYABLE_H

#include <memory>
#include <utility>

// 禁止拷貝
class non_copyable {
 protected:
    non_copyable() = default;

 public:
    non_copyable(const non_copyable &) = delete;
    non_copyable &operator=(const non_copyable &) = delete;
};

// cpp11只有make_shared，缺少make_unique cpp14才有
template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif