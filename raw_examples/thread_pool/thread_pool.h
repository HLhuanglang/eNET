#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include "msg.h"
#include "msg_qunue.h"

class thread_pool {
 public:
    thread_pool(size_t max_items, size_t thread_cnt, const std::function<void()> &on_thread_start, const std::function<void()> &on_thread_stop) : m_msg_queue(max_items) {
        auto cpu_cnt = std::thread::hardware_concurrency();
        if (thread_cnt == 0 || thread_cnt > cpu_cnt * 2) {
            std::cerr << "thread_cnt " << thread_cnt << " is invalid: (0," << cpu_cnt * 2 << "]" << std::endl;
            exit(-1);
        }
        for (int i = 0; i < thread_cnt; i++) {
            m_threads.emplace_back([this, on_thread_start, on_thread_stop] {
                if (on_thread_start) {
                    on_thread_start();
                }
                this->_work_loop();
                if (on_thread_stop) {
                    on_thread_stop();
                }
            });
        }
    }

    ~thread_pool() {
        for (int i = 0; i < m_threads.size(); i++) {
            for (auto &it : m_threads) {
                it.join();  // 主线程需等待子线程执行完成
            }
        }
    }

 public:
    void commit(test_msg msg) {
        m_msg_queue.enqueue(std::move(msg));  // 提交任务到线程池,并唤醒一个线程
    }

 private:
    void _work_loop() {
        while (_prcess_next_msg()) {
            // 确保工作线程只在线程池释放或者主动设置关闭时才结束
        };
    }

    bool _prcess_next_msg() {
        // fixme：增加不同类型的消息,有些任务使得线程执行完了就退出
        test_msg msg;
        m_msg_queue.dequeue(msg);  // 阻塞在这里,等待生产者投递消息
        msg.consume();
        return true;
    }

 private:
    std::vector<std::thread> m_threads;
    msg_queue<test_msg> m_msg_queue;
};