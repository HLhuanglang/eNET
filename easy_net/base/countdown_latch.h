#ifndef __EASYNET_COUNTDOWN_LATCH_H
#define __EASYNET_COUNTDOWN_LATCH_H

#include <assert.h>

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace EasyNet {
///@brief 减法计数器
///@note 统计有多少个线程被执行了,当所有线程都执行完毕后,才能继续执行主线程
class CountDownLatch {
 public:
    explicit CountDownLatch(int count) : m_count(count) {
        assert(count > 0);
    }

    // 禁止拷贝构造和赋值
    // 用于线程之间同步,所有线程共享一个CountDownLatch对象,不允许拷贝
    // 一般使用引用或者指针传递给线程
    CountDownLatch(const CountDownLatch &) = delete;
    CountDownLatch &operator=(const CountDownLatch &) = delete;

 public:
    // 等待，直至count减为0
    void Wait() {
        if (m_count == 0) {
            return;
        }
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv.wait(lock, [&]() {
            return m_count == 0;
        });
    }

    // count减1，（最小为0）
    void CountDown() {
        int old_c = m_count.load();
        while (old_c > 0) {
            // 尝试将计数器的值减1(原子操作,将m_count值从old_c更新为old_c-1,更新成功返回true)
            if (m_count.compare_exchange_strong(old_c, old_c - 1)) {
                if (old_c == 1) {
                    std::unique_lock<std::mutex> lock(m_mtx);
                    m_cv.notify_all();
                }
                break;  // 成功减1后退出循环
            }
            old_c = m_count.load();  // 如果减1失败，重新读取计数器的值并重试
        }
    }

 private:
    std::atomic<int> m_count;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};
}  // namespace EasyNet

#endif  // !__EASYNET_COUNTDOWN_LATCH_H
