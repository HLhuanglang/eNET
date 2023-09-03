#ifndef __EASYNET_MSG_H
#define __EASYNET_MSG_H

#include <iostream>
#include <string>
#include <thread>
#include <utility>

class test_msg {
 public:
    test_msg() = default;
    test_msg(test_msg &&msg) = default;
    test_msg(const test_msg &msg) = default;
    test_msg &operator=(test_msg &&) = default;

 public:
    void set_msg(std::string msg) {
        this->m_msg = std::move(msg);
    }
    void consume() {
        std::cout << "consume: " << m_msg << " " << std::this_thread::get_id() << std::endl;
    }

    friend std::ostream &operator<<(std::ostream &os, const test_msg &msg) {
        os << "{ " << msg.m_msg << " }";
        return os;
    }

 private:
    std::string m_msg;
};

#endif // !__EASYNET_MSG_H
