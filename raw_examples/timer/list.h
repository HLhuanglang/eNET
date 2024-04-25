#ifndef __EASYNET_LIST_H
#define __EASYNET_LIST_H

#include <ctime>
#include <fcntl.h>
#include <list>
#include <sys/epoll.h>

#include "timer.h"

// 有序链表的定时器
// 排序规则是采用定时间隔的
// 1s-2s-2s-3s-4s

class list_timer : public timer_policy<std::list<timer>> {
 public:
    std::list<timer> &get_timers() override {
        return m_timers;
    }

    int find_timer() override {
        if (m_timers.empty()) {
            return 1 * 1000; // 默认1s,如果直接返回0,会导致epoll_wait立即返回,整个进程的CPU占用率会很高
        }
        return m_timers.front().get_interval();
    }

    void add_timer(int tm, timer_type type, const timer::timer_cb &cb) override {
        if (m_timers.empty()) {
            m_timers.emplace_back(_gen_timer_id(), type, cb, tm);
        } else {
            for (auto it = m_timers.begin(); it != m_timers.end(); ++it) {
                if (tm < it->get_interval()) {
                    m_timers.insert(it, {_gen_timer_id(), type, cb, tm});
                    return;
                }
            }
            m_timers.emplace_back(_gen_timer_id(), type, cb, tm);
        }
    }

    void handle_expired_timer() override {
        // 处理过期的定时器
        for (auto it = m_timers.begin(); it != m_timers.end(); ++it) {
            // 先对所有的定时器进行时间累加。
            // fixme：如果存在网络IO事件，在处理IO事件时耗费了很多时间，那么这种简单的时间累加就会导致定时器的误差变大
            // 是否可以增加一个任务队列，超时事件直接丢给另外一个工作线程去处理？
            // 这样子的意义不大，因为定时任务耗时的话，任务队列同样会累计时间误差......
            // 也就是说，事件驱动的框架中，定时任务绝对不能很耗时。

            it->update_curr_time();

            if (it->is_expired()) {
                switch (it->get_type()) {
                    case timer_type::E_EVERY: {
                        it->run_task();
                        it->get_next_expired_time();
                        break;
                    }
                    case timer_type::E_AFTER: {
                        it->run_task();
                        // m_timers.remove(*it++); //remove需要遍历整个链表,使用earse是直接根据位置进行删除的.
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
                        m_timers.erase(it++);
                        break;
                    }
                    case timer_type::E_AT:
                        // TODO
                        break;
                }
            }
        }
    }
};

#endif // !__EASYNET_LIST_H
