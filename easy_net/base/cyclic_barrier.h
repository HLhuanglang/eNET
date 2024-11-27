#ifndef __EASYNET_CYCLIC_BARRIER_H
#define __EASYNET_CYCLIC_BARRIER_H

#include <assert.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>

namespace EasyNet {
///@brief 循环加法计数器
///@note 用于多个线程之间的同步,当所有线程都到达屏障后,才能继续执行
class CyclicBarrier {
 public:
    typedef std::function<void()> OnCompletion;

 public:
    explicit CyclicBarrier(int count) : m_expected_count(count) {
        assert(m_expected_count > 0);
    }

    explicit CyclicBarrier(int count, OnCompletion on_completion)
        : m_expected_count(count), m_on_completion_fn(on_completion) {
        assert(m_expected_count > 0);
    }

    CyclicBarrier(const CyclicBarrier &) = delete;
    CyclicBarrier &operator=(const CyclicBarrier &) = delete;

 public:
    ///@brief 到达屏障,将计数器加1
    void Arrive() {
        // TODO
    }

    ///@brief 等待当前循环中的所有线程到达屏障(阻塞住)
    void Wait() {
        // TODO
    }

    ///@brief 到达屏障并减少期待数,然后等待
    void ArriveAndWait() {
        // TODO
    }

    ///@brief 到达屏障,将后继阶段的初始期待计数和当前阶段的期待计数均减少一
    void ArriveAndDrop() {
        // TODO
    }

 private:
    OnCompletion m_on_completion_fn;  // 当前线程到达屏障后的回调函数
    size_t m_cyclic_count;            // 循环的次数
    size_t m_expected_count;          // 当前循环的期待数
    std::atomic<int> m_count;         // 到达屏障的线程数
    std::mutex m_mtx;
    std::condition_variable m_cv;
};
}  // namespace EasyNet

#endif  // !__EASYNET_CYCLIC_BARRIER_H
