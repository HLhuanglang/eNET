#ifndef __EASYNET_CYCLIC_BARRIER_H
#define __EASYNET_CYCLIC_BARRIER_H

#include <assert.h>

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace EasyNet {
///@brief 循环加法计数器
///@note 用于多个线程之间的同步,当所有线程都到达屏障后,才能继续执行
class CyclicBarrier {
 public:
    explicit CyclicBarrier(int count) : m_count(count) {
        assert(count > 0);
    }

    CyclicBarrier(const CyclicBarrier &) = delete;
    CyclicBarrier &operator=(const CyclicBarrier &) = delete;

 public:
 private:
    std::atomic<int> m_count;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};
}  // namespace EasyNet

#endif  // !__EASYNET_CYCLIC_BARRIER_H
