#ifndef __EASYNET_TIMER_POLICY_H
#define __EASYNET_TIMER_POLICY_H

#include <ctime>
#include <list>
#include <queue>

#include "def.h"
#include "timer.h"

namespace EasyNet {

template <typename TimerContainer>
class TimerPolicy {
 public:
    virtual ~TimerPolicy() = default;
    virtual TimerID AddTimer(int tm, TimerType type, const TimerCallBack &cb) = 0;
    virtual void CancelTimer(TimerID id) = 0;
    virtual int GetNextExpiredTime() = 0;
    virtual void HandleExpiredTimer() = 0;

 protected:
    static TimerID GenTimerID() {
        static TimerID s_timer_id = 0;
        return ++s_timer_id;
    }

 protected:
    TimerContainer m_timers;
};

// 队头是最早的定时器
// PS：100ms->200ms->200ms->500ms->1s
class ListTimer : public TimerPolicy<std::list<Timer>> {
 public:
    int GetNextExpiredTime() override {
        if (m_timers.empty()) {
            return KDefaultWaitTimeMS;  // 默认1s,如果直接返回0,会导致epoll_wait立即返回,整个进程的CPU占用率会很高
        }
        // 可能返回0,需要立刻执行把堆积的过期定时器全部处理完.
        auto next_expired_timer = m_timers.front();
        auto tm = next_expired_timer.get_expired_time();
        return tm;
    }

    TimerID AddTimer(int tm, TimerType type, const TimerCallBack &cb) override {
        auto id = GenTimerID();
        if (m_timers.empty()) {
            m_timers.emplace_back(id, type, cb, tm);
        } else {
            for (auto it = m_timers.begin(); it != m_timers.end(); ++it) {
                if (tm < it->get_expired_time()) {
                    m_timers.insert(it, {id, type, cb, tm});
                    return id;
                }
            }
            m_timers.emplace_back(id, type, cb, tm);
        }
        return id;
    }

    void CancelTimer(TimerID id) override {
        m_timers.remove_if([id](const Timer &t) {
            return t.get_id() == id;
        });
    }

    void HandleExpiredTimer() override {
        for (auto it = m_timers.begin(); it != m_timers.end(); ++it) {
            it->update_curr_time();
            if (it->expired()) {
                switch (it->get_type()) {
                    case TimerType::E_EVERY: {
                        it->run_task();
                        it->update_expired_time();  // 只更新过期时间,不重排序(迭代器失效问题)
                        it->update_curr_time();
                        break;
                    }
                    case TimerType::E_AFTER: {
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
                    case TimerType::E_AT:
                        // TODO
                        break;
                }
            }
        }

        // 重新排序
        m_timers.sort([](const Timer &lhs, const Timer &rhs) {
            return lhs.get_expired_time() < rhs.get_expired_time();
        });
    }
};

// 基于最小堆的定时器
// 堆顶是最近要过期的时间
//       1s
//        /\
//     2s 3s
//      /\   \
//   4s 5s  6s
class MiniHeapTimer : public TimerPolicy<std::priority_queue<Timer, std::vector<Timer>, TimerGreater>> {
 public:
    int GetNextExpiredTime() override {
        if (m_timers.empty()) {
            return KDefaultWaitTimeMS;  // 默认10ms,如果直接返回0,会导致epoll_wait立即返回,整个进程的CPU占用率会很高
        }
        // 可能返回0,需要立刻执行把堆积的过期定时器全部处理完.
        auto next_expired_timer = m_timers.top();
        auto tm = next_expired_timer.get_expired_time();
        return tm;
    }

    TimerID AddTimer(int tm, TimerType type, const TimerCallBack &cb) override {
        auto id = GenTimerID();
        m_timers.emplace(id, type, cb, tm);  // 插入元素时，排序的规则是根据过期时间来进行判断的
        return id;
    }

    void CancelTimer(TimerID id) override {
        std::priority_queue<Timer, std::vector<Timer>, TimerGreater> tmp;
        while (!m_timers.empty()) {
            auto t = m_timers.top();
            m_timers.pop();
            if (t.get_id() != id) {
                tmp.push(t);
            }
        }
        m_timers.swap(tmp);
    }

    void HandleExpiredTimer() override {
        while (!m_timers.empty()) {
            auto t = m_timers.top();
            t.update_curr_time();
            if (t.expired()) {
                switch (t.get_type()) {
                    case TimerType::E_EVERY: {
                        t.run_task();
                        t.update_curr_time();
                        t.update_expired_time();
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

class TimeWheelTimer {
    // TODO
};

}  // namespace EasyNet

#endif  // !__EASYNET_TIMER_POLICY_H
