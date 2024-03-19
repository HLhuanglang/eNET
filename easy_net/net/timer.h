#ifndef __EASYNET_TIMER_H
#define __EASYNET_TIMER_H

#include <cstdint>
#include <ctime>
#include <fcntl.h>
#include <functional>
#include <list>
#include <sys/epoll.h>
#include <utility>

#include <iostream>

#include "def.h"

namespace EasyNet {

const constexpr int g_max_timeout = 5 * 1000;

enum TimerType {
    E_EVERY,
    E_AFTER,
    E_AT
};

class Timer {
 public:
 public:
    Timer(uint64_t id, TimerType type, TimerCallBack cb, int interval)
        : m_id(id), m_type(type), m_interval(interval), m_cb(std::move(cb)) {
        clock_gettime(CLOCK_REALTIME, &m_current_time);
        m_expried_time.tv_sec = m_current_time.tv_sec + m_interval / 1000;
        m_expried_time.tv_nsec = m_current_time.tv_nsec + (m_interval % 1000) * 1000 * 1000;
    }

    // 用于remove时进行比较
    bool operator==(const Timer &rhs) const {
        return m_id == rhs.m_id;
    }

    bool operator>(const Timer &rhs) const {
        return m_expried_time.tv_sec > rhs.m_expried_time.tv_sec ||
               (m_expried_time.tv_sec == rhs.m_expried_time.tv_sec &&
                m_expried_time.tv_nsec >= rhs.m_expried_time.tv_nsec);
    }

 public:
    bool is_expired() const {
        bool ret = false;
        switch (m_type) {
            case TimerType::E_AFTER:
            case TimerType::E_EVERY: {
                auto diff_s = m_current_time.tv_sec - m_expried_time.tv_sec;
                auto diff_ms = m_current_time.tv_nsec / 1000000 - m_expried_time.tv_nsec / 1000000;
                if (diff_s > 0 || (diff_s == 0 && diff_ms >= 0)) {
                    ret = true;
                }
                break;
            }
            case TimerType::E_AT: {
                ret = false;
                break;
            }
        }
        return ret;
    }

    // 使用当前时间更新下一次超时时间,避免超时任务带来的时间误差
    // 例如设置的是run_every(2s,task)
    // 预期：16:00任务开始---->16:02触发,执行完任务16:02----->16:04触发,执行完任务16:06
    // 实际：16:00任务开始---->16:02触发,执行完任务16:03----->16:05触发,执行完任务16:10
    void get_next_expired_time() {
        clock_gettime(CLOCK_REALTIME, &m_expried_time);
        auto s = m_interval / 1000;
        auto ms = m_interval % 1000;
        m_expried_time.tv_sec += s;
        m_expried_time.tv_nsec += ms * 1000 * 1000;
    }

    void update_curr_time() {
        clock_gettime(CLOCK_REALTIME, &m_current_time);
    }

    void run_task() { m_cb(); }
    int get_interval() const { return m_interval; }
    int get_expired_time() const { return (m_expried_time.tv_sec - m_current_time.tv_sec) / 1000 + (m_expried_time.tv_nsec - m_current_time.tv_nsec) / 1000000; }
    void set_type(TimerType type) { m_type = type; }
    TimerType get_type() const { return m_type; }
    int get_id() const { return m_id; }

 private:
    int m_id;
    TimerCallBack m_cb;
    timespec m_expried_time; // 下一次过期的时间戳
    timespec m_current_time; // 当前时间戳
    TimerType m_type;
    int m_timeat;   // run_at
    int m_interval; // 超时时间间隔  单位：ms
};

template <typename Container>
class TimerPolicy {
 public:
    virtual ~TimerPolicy() = default;
    virtual Container &get_timers() = 0;
    virtual int find_timer() = 0;
    virtual void add_timer(int tm, TimerType type, const TimerCallBack &cb) = 0;
    virtual void handle_expired_timer() = 0;

 protected:
    static uint64_t _gen_timer_id() {
        static uint64_t s_timer_id = 0;
        return ++s_timer_id;
    }

 protected:
    Container m_timers;
};

template <typename Policy>
class TimerManager {
 public:
    void run_at(unsigned long long tm, const TimerCallBack &cb) {
        m_timer_container.add_timer(tm, TimerType::E_AT, cb);
    }
    void run_after(unsigned long long tm, const TimerCallBack &cb) {
        m_timer_container.add_timer(tm, TimerType::E_AFTER, cb);
    }

    void run_every(unsigned long long tm, const TimerCallBack &cb) {
        m_timer_container.add_timer(tm, TimerType::E_EVERY, cb);
    }

    int next_timeout() {
        return m_timer_container.find_timer();
    }

    void handle_expired_timer() {
        m_timer_container.handle_expired_timer();
    }

 private:
    Policy m_timer_container;
};
} // namespace EasyNet

#endif // !__EASYNET_TIMER_H
