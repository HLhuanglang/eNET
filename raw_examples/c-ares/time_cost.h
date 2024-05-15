#ifndef __EASYNET_TIME_COST_H
#define __EASYNET_TIME_COST_H

#include <chrono>
#include <iostream>

class ScopedTimer {
 public:
    ScopedTimer(const char* name) : m_name(name), m_begin(std::chrono::high_resolution_clock::now()) {}
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_begin);
        std::cout << m_name << " : " << dur.count() << " ns\n";
    }

 private:
    const char* m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
};

#endif  // !__EASYNET_TIME_COST_H
