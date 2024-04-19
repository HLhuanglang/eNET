#ifndef __EASYNET_MINIHEAP_H
#define __EASYNET_MINIHEAP_H

#include <ctime>
#include <fcntl.h>
#include <queue>
#include <sys/epoll.h>

#include <vector>

#include "def.h"
#include "timer.h"

// 基于最小堆的定时器
// 堆顶一顶是即将过期的定时器
//       1s
//        /\
//     2s 3s
//      /\   \
//   4s 5s  6s

namespace EasyNet {

class compare {
 public:
    bool operator()(const Timer &a, const Timer &b) {
        return a > b;
    }
};

class MiniHeapTimer : public TimerPolicy<std::priority_queue<Timer, std::vector<Timer>, compare>> {
 public:
    std::priority_queue<Timer, std::vector<Timer>, compare> &get_timers() override {
        return m_timers;
    }

    int find_timer() override {
        if (m_timers.empty()) {
            return 1 * 1000; // 默认1s,如果直接返回0,会导致epoll_wait立即返回,整个进程的CPU占用率会很高
        }
        return m_timers.top().get_expired_time(); // 不能使用get_interval,因为get_interval是下一次超时的时间间隔,而不是当前的实际超时时间
    }

    void add_timer(int tm, TimerType type, const TimerCallBack &cb) override {
        m_timers.emplace(_gen_timer_id(), type, cb, tm); // 插入元素时，排序的规则是根据过期时间来进行判断的
    }

    void handle_expired_timer() override {
        while (!m_timers.empty()) {
            auto t = m_timers.top();
            t.update_curr_time();
            if (t.is_expired()) {
                switch (t.get_type()) {
                    case TimerType::E_EVERY: {
                        t.run_task();
                        t.get_next_expired_time();
                        m_timers.pop();
                        m_timers.push(t);
                        break;
                    }
                    case TimerType::E_AFTER: {
                        t.run_task();
                        m_timers.pop();
                        break;
                    }
                    case TimerType::E_AT:
                        // TODO
                        break;
                }
            } else {
                return;
            }
        }
    }
};
} // namespace EasyNet

#endif // !__EASYNET_MINIHEAP_H
