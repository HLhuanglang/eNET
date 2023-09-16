#ifndef __EASYNET_SIGLETON_H
#define __EASYNET_SIGLETON_H

#include <memory>
#include <mutex>

template <typename T>
class sigleton {
 public:
    static T *get_instance() {
        if (nullptr == instance) {
            std::call_once(once_f, [&]() {
                sigleton::instance.reset(new T());
            });
        }
        return instance.get();
    }

 public:
    sigleton() = delete;
    sigleton(const sigleton &rhs) = delete;
    sigleton &operator=(const sigleton &rhs) = delete;

 private:
    static std::unique_ptr<T> instance;
    static std::once_flag once_f;
};

template <typename T>
std::unique_ptr<T> sigleton<T>::instance = nullptr;

template <typename T>
std::once_flag sigleton<T>::once_f;

#endif