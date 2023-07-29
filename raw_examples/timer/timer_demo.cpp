#include "list.h"
#include "miniheap.h"
#include "some_time_func.h"
#include "timewheel.h"

#include <iostream>
#include <unistd.h>

//后缀的参数只能是unsigned long long、long double、const char*或者const char* + size_t
unsigned long long operator"" _s(unsigned long long s) {
    return s * 1000;
}

unsigned long long operator"" _ms(unsigned long long ms) {
    return ms;
}

int main() {
    list::timer_manager tm;

    tm.run_after(10_s, [](int) {
        std::cout << "10s" << std::endl;
    });

    tm.run_every(1_s, [](int) {
        std::cout << "1s" << std::endl;
    });

    tm.run_every(5_s, [](int) {
        std::cout << "5s" << std::endl;
    });

    tm.start();
}