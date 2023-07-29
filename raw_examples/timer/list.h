#ifndef __EASYNET_LIST_H
#define __EASYNET_LIST_H

#include <ctime>
#include <fcntl.h>
#include <functional>
#include <list>
#include <sys/epoll.h>
#include <utility>

#include <iostream>

//基于链表的定时器
namespace list {

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

 public:
    bool is_expired() const {
        bool ret = false;
        switch (m_type) {
            case timer_type::E_AFTER:
            case timer_type::E_EVERY: {
                auto diff_s = m_current_time.tv_sec - m_expried_time.tv_sec;
                auto diff_ms = m_current_time.tv_nsec / 1000000 - m_expried_time.tv_nsec / 1000000;
                std::cout << "diff_s:" << diff_s << " diff_ms:" << diff_ms << std::endl;
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

    void
    updatetime() {
        auto s = m_interval / 1000;
        auto ms = m_interval % 1000;
        m_expried_time.tv_sec += s;
        m_expried_time.tv_nsec += ms * 1000 * 1000;
    }

    void add_elapse(int tm) {
        auto s = tm / 1000;
        auto ms = tm % 1000;
        m_current_time.tv_sec += s;
        m_current_time.tv_nsec += ms * 1000 * 1000;
    }

    void
    run_task() { m_cb(m_interval); }
    int get_interval() const { return m_interval; }
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

class timer_list {
    friend class timer_manager;

 public:
    int find_timer() {
        if (m_timer_list.empty()) {
            return 1 * 1000; //默认1s,如果直接返回0,会导致epoll_wait立即返回,整个进程的CPU占用率会很高
        }
        return m_timer_list.front().get_interval();
    }

    void add_timer(int tm, timer_type type, const timer::timer_cb &cb) {
        if (m_timer_list.empty()) {
            m_timer_list.emplace_back(timer{_gen_timer_id(), type, cb, tm});
        } else {
            for (auto it = m_timer_list.begin(); it != m_timer_list.end(); ++it) {
                if (tm < it->get_interval()) {
                    m_timer_list.insert(it, {_gen_timer_id(), type, cb, tm});
                    return;
                }
            }
            m_timer_list.emplace_back(timer{_gen_timer_id(), type, cb, tm});
        }
    }

 private:
    static int _gen_timer_id() {
        static int s_timer_id = 0;
        return ++s_timer_id;
    }

 private:
    std::list<timer> m_timer_list;
};

class timer_manager {
 public:
    void run_at(unsigned long long tm, const std::function<void(int)> &cb) {
        m_tl.add_timer(tm, timer_type::E_AT, cb);
    }
    void run_after(unsigned long long tm, const std::function<void(int)> &cb) {
        m_tl.add_timer(tm, timer_type::E_AFTER, cb);
    }

    void run_every(unsigned long long tm, const std::function<void(int)> &cb) {
        m_tl.add_timer(tm, timer_type::E_EVERY, cb);
    }

    void start() {
        int epollfd = ::epoll_create1(0);
        const int k_max_events = 1024;
        struct epoll_event ee[1024];

        while (true) {
            //等待定时器超时
            int timeout = 0;
            while (true) {
                timeout = m_tl.find_timer();

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

            // 处理过期的定时器
            for (auto it = m_tl.m_timer_list.begin(); it != m_tl.m_timer_list.end(); ++it) {
                // 先对所有的定时器进行时间累加。
                // fixme：如果存在网络IO事件，在处理IO事件时耗费了很多时间，那么这种简单的时间累加就会导致定时器的误差变大
                // 是否可以增加一个任务队列，超时事件直接丢给另外一个工作线程去处理？
                // 这样子的意义不大，因为定时任务耗时的话，任务队列同样会累计时间误差......
                // 也就是说，事件驱动的框架中，定时任务绝对不能很耗时。

                it->add_elapse(timeout);

                if (it->is_expired()) {
                    switch (it->get_type()) {
                        case timer_type::E_EVERY: {
                            it->updatetime(); //先更新一下次过期时间，确保下一次过期时间是正确的，避免执行耗时任务带来的影响
                            it->run_task();
                            break;
                        }
                        case timer_type::E_AFTER: {
                            it->run_task();
                            // m_tl.m_timer_list.remove(*it++); //需要遍历整个链表,使用earse是直接根据位置进行删除的.
                            /*
                            template <class _Tp, class _Alloc>
                            void list<_Tp, _Alloc>::remove(const _Tp& __value)
                            {
                                iterator __first = begin();
                                iterator __last = end();
                                while (__first != __last) {
                                    iterator __next = __first;
                                    ++__next;
                                    if (*__first == __value) erase(__first);    //使用remove,在这一步需要用到==操作符
                                    __first = __next;
                                }
                            }
                            */
                            m_tl.m_timer_list.erase(it++);
                            break;
                        }
                        case timer_type::E_AT:
                            // TODO
                            break;
                    }
                }
            }
        }
    }

 private:
    timer_list m_tl;
};

} // namespace list

#endif // !__EASYNET_LIST_H
