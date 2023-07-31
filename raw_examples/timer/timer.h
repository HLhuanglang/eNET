#ifndef __EASYNET_TIMER_H
#define __EASYNET_TIMER_H

#include <ctime>
#include <fcntl.h>
#include <functional>
#include <list>
#include <sys/epoll.h>
#include <utility>

#include <iostream>

enum timer_type {
    E_EVERY,
    E_AFTER,
    E_AT
};

const int g_k_max_timeout = 5 * 1000;

class timer {
 public:
    using timer_cb = std::function<void(int)>;

 public:
    timer(int id, timer_type type, timer_cb cb, int interval)
        : m_id(id), m_type(type), m_interval(interval), m_cb(std::move(cb)) {
        clock_gettime(CLOCK_REALTIME, &m_current_time);
        m_expried_time.tv_sec = m_current_time.tv_sec + m_interval / 1000;
        m_expried_time.tv_nsec = m_current_time.tv_nsec + (m_interval % 1000) * 1000 * 1000;
    }

    //用于remove时进行比较
    bool operator==(const timer &rhs) const {
        return m_id == rhs.m_id;
    }

    bool operator>(const timer &rhs) const {
        return m_expried_time.tv_sec > rhs.m_expried_time.tv_sec ||
               (m_expried_time.tv_sec == rhs.m_expried_time.tv_sec &&
                m_expried_time.tv_nsec >= rhs.m_expried_time.tv_nsec);
    }

 public:
    bool is_expired() const {
        bool ret = false;
        switch (m_type) {
            case timer_type::E_AFTER:
            case timer_type::E_EVERY: {
                auto diff_s = m_current_time.tv_sec - m_expried_time.tv_sec;
                auto diff_ms = m_current_time.tv_nsec / 1000000 - m_expried_time.tv_nsec / 1000000;
                if (diff_s > 0 || (diff_s == 0 && diff_ms >= 0)) {
                    ret = true;
                }
                break;
            }
            case timer_type::E_AT: {
                ret = false;
                break;
            }
        }
        return ret;
    }

    //使用当前时间更新下一次超时时间,避免超时任务带来的时间误差
    //例如设置的是run_every(2s,task)
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

    void run_task() { m_cb(m_interval); }
    int get_interval() const { return m_interval; }
    int get_expired_time() const { return (m_expried_time.tv_sec - m_current_time.tv_sec) / 1000 + (m_expried_time.tv_nsec - m_current_time.tv_nsec) / 1000000; }
    void set_type(timer_type type) { m_type = type; }
    timer_type get_type() const { return m_type; }
    int get_id() const { return m_id; }

 private:
    int m_id;
    timer_cb m_cb;
    timespec m_expried_time; //下一次过期的时间戳
    timespec m_current_time; //当前时间戳
    timer_type m_type;
    int m_timeat;   // run_at
    int m_interval; //超时时间间隔  单位：ms
};

template <typename Container>
class timer_policy {
 public:
    virtual ~timer_policy() = default;
    virtual Container &get_timers() = 0;
    virtual int find_timer() = 0;
    virtual void add_timer(int tm, timer_type type, const timer::timer_cb &cb) = 0;
    virtual void handle_expired_timer() = 0;

 protected:
    static int _gen_timer_id() {
        static int s_timer_id = 0;
        return ++s_timer_id;
    }

 protected:
    Container m_timers;
};

template <typename Policy>
class timer_manager {
 public:
    void run_at(unsigned long long tm, const std::function<void(int)> &cb) {
        m_timer_container.add_timer(tm, timer_type::E_AT, cb);
    }
    void run_after(unsigned long long tm, const std::function<void(int)> &cb) {
        m_timer_container.add_timer(tm, timer_type::E_AFTER, cb);
    }

    void run_every(unsigned long long tm, const std::function<void(int)> &cb) {
        m_timer_container.add_timer(tm, timer_type::E_EVERY, cb);
    }

    void start() {
        int epollfd = ::epoll_create1(0);
        const int k_max_events = 1024;
        struct epoll_event ee[1024];

        while (true) {
            //等待定时器超时
            while (true) {
                int timeout = m_timer_container.find_timer();

                //可能存在定时任务非常大，导致timeout溢出，这里做一下处理
                //或者提供一个接口，让用户设定ticks的大小
                if (timeout >= g_k_max_timeout) {
                    timeout = g_k_max_timeout;
                }

                // timeout -1：阻塞, 会一直等待IO事件到达，如果没有就一直阻塞在这里。
                // timeout 0：不阻塞，立即返回，如果没有IO事件到达，就返回0
                int n = epoll_wait(epollfd, ee, k_max_events, timeout);
                if (n < 0 && errno == EINTR) {
                    continue;
                }
                //如果是在网络编程中，这里还需与处理IO事件
                break;
            }

            m_timer_container.handle_expired_timer();
        }
    }

 private:
    Policy m_timer_container;
};

#endif // !__EASYNET_TIMER_H
