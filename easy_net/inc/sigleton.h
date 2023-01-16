#ifndef __EASYNET_SIGLETON_H
#define __EASYNET_SIGLETON_H

#include <memory>
#include <mutex>

template <typename T>
class sigleton {
public:
    static T *get_instance() {
        if (nullptr == instance_) {
            std::call_once(once_f_, [&]() {
                sigleton::instance_.reset(new T());
            });
        }
        return instance_.get();
    }

private:
    sigleton();
    sigleton(const sigleton &rhs) = delete;
    sigleton &operator=(const sigleton &rhs) = delete;

private:
    static std::unique_ptr<T> instance_;
    static std::once_flag once_f_;
};

template <typename T>
std::unique_ptr<T> sigleton<T>::instance_ = nullptr;

template <typename T>
std::once_flag sigleton<T>::once_f_;

#endif