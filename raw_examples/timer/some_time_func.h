#ifndef __EASYNET_SOME_TIME_FUNC_H
#define __EASYNET_SOME_TIME_FUNC_H

#include <ctime>
#include <iostream>
#include <sys/time.h> //gettimeofday()
#include <unistd.h>

//时间换算关系
// 1s[second] = 1000ms[millisecond ] = 1000,000us[microsecond] = 1000,000,000ns[nanosecond]
inline void some_function_to_get_curr_time() {

    // (bt) time()函数,C库函数,精度秒,返回自1970年1月1日以来经过的秒数
    // https://man7.org/linux/man-pages/man2/time.2.html
    time_t t;
    t = time(nullptr);
    std::cout << t << std::endl;

    // ftime()函数,精度毫秒。C库函数，不过已经被弃用了
    // https://man7.org/linux/man-pages/man3/ftime.3.html
    //
    //

    // (bt)gettimeofday(), 精度微妙，C库函数
    // https://man7.org/linux/man-pages/man2/settimeofday.2.html
    timeval tv;
    gettimeofday(&tv, nullptr);
    std::cout << tv.tv_sec << "s " << tv.tv_usec << "us " << std::endl;

    // (bt)clock_gettime(), 精度纳秒，C库函数
    // https://man7.org/linux/man-pages/man2/clock_gettime.2.html
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    std::cout << ts.tv_sec << "s " << ts.tv_nsec << "ns " << std::endl;

    // Time Stamp Counter
    // https://stackoverflow.com/questions/42436059/using-time-stamp-counter-to-get-the-time-stamp
    //
    //
}

inline void some_function_to_impl_timer() {
    // sleep(), 精度秒，C库函数
    // https://man7.org/linux/man-pages/man3/sleep.3.html
    sleep(5);

    // usleep(), 精度微妙，C库函数
    //
    usleep(1000);

    // nanosleep(), 精度纳秒，C库函数
    timespec ts;
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    nanosleep(&ts, nullptr);
}

#endif // !__EASYNET_SOME_TIME_FUNC_H
