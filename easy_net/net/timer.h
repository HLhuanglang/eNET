#ifndef __EASYNET_TIMER_H
#define __EASYNET_TIMER_H

#include <fcntl.h>
#include <sys/epoll.h>

#include <cstdint>
#include <ctime>
#include <functional>

#include "def.h"

namespace EasyNet {

enum TimerType {
    E_EVERY,
    E_AFTER,
    E_AT
};

using TimerID = uint64_t;

class Timer {
 public:
    Timer(uint64_t id, TimerType type, TimerCallBack cb, int interval)
        : m_id(id), m_type(type), m_interval(interval), m_cb(cb) {
        clock_gettime(CLOCK_REALTIME, &s_current_time);
        m_expried_time.tv_sec = s_current_time.tv_sec + m_interval / 1000;
        m_expried_time.tv_nsec = s_current_time.tv_nsec + (m_interval % 1000) * 1000 * 1000;
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
    bool expired() const {
        bool ret = false;
        switch (m_type) {
            case TimerType::E_AFTER:
            case TimerType::E_EVERY: {
                auto diff_s = s_current_time.tv_sec - m_expried_time.tv_sec;
                auto diff_ms = s_current_time.tv_nsec / 1000000 - m_expried_time.tv_nsec / 1000000;
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
    void update_expired_time() {
        clock_gettime(CLOCK_REALTIME, &m_expried_time);
        auto s = m_interval / 1000;
        auto ms = m_interval % 1000;
        m_expried_time.tv_sec += s;
        m_expried_time.tv_nsec += ms * 1000 * 1000;
    }

    // 返回ms
    int get_expired_time() const {
        auto tm = (m_expried_time.tv_sec - s_current_time.tv_sec) * 1000 +
                  (m_expried_time.tv_nsec - s_current_time.tv_nsec) / 1000000;
        if (tm < 0) {
            tm = 0;
        }
        return tm;
    }

    void update_curr_time() {
        clock_gettime(CLOCK_REALTIME, &s_current_time);
    }

    void run_task() { m_cb(); }
    int get_interval() const { return m_interval; }
    void set_type(TimerType type) { m_type = type; }
    TimerType get_type() const { return m_type; }
    TimerID get_id() const { return m_id; }
    TimerCallBack get_cb() const { return m_cb; }

 public:
    static timespec s_current_time;  // 当前时间戳(应该全局,所有定时器共用)

 private:
    TimerID m_id;
    TimerCallBack m_cb;
    timespec m_expried_time;  // 下一次过期的时间戳
    TimerType m_type;
    int m_timeat;    // run_at
    int m_interval;  // 超时时间间隔  单位：ms
};

class TimerGreater {
 public:
    bool operator()(const Timer &a, const Timer &b) {
        return a > b;
    }
};

}  // namespace EasyNet

#endif  // !__EASYNET_TIMER_H
