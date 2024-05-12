#ifndef __EASYNET_SIGLETON_H
#define __EASYNET_SIGLETON_H

#include <memory>
#include <mutex>

namespace EasyNet {

template <typename T>
class Singleton {
 public:
    static T *GetInstance() {
        if (nullptr == instance) {
            std::call_once(once_f, [&]() {
                Singleton::instance.reset(new T());
            });
        }
        return instance.get();
    }

 public:
    Singleton() = delete;
    Singleton(const Singleton &rhs) = delete;
    Singleton &operator=(const Singleton &rhs) = delete;

 private:
    static std::unique_ptr<T> instance;
    static std::once_flag once_f;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;

template <typename T>
std::once_flag Singleton<T>::once_f;

}  // namespace EasyNet

#endif