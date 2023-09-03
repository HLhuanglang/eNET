/*
   阻塞消息队列
*/
#ifndef __EASYNET_MSG_QUNUE_H
#define __EASYNET_MSG_QUNUE_H

#include "data_container.h"
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>

template <typename T>
class msg_queue {
 public:
    using item_type = T;
    explicit msg_queue(size_t max_items)
        : m_data(max_items) {}

    void enqueue(T &&item) {
        {
            std::unique_lock<std::mutex> lock(m_queue_mtx);
            std::cout << "enqueue waiting..." << std::this_thread::get_id() << std::endl;
            m_pop_cv.wait(lock, [this] {
                return !this->m_data.full(); // 如果没空间了,就一直阻塞,除非被唤醒
            });
            m_data.push_back(std::move(item));
        }
        std::cout << "enqueue notify..." << std::this_thread::get_id() << std::endl;
        m_push_cv.notify_one();
    }

    // enqueue immediately. overrun oldest message in the queue if no room left.
    void enqueue_nowait(T &&item) {
        {
            std::unique_lock<std::mutex> lock(m_queue_mtx);
            m_data.push_back(std::move(item));
        }
        m_push_cv.notify_one();
    }

    // dequeue with a timeout.
    // Return true, if succeeded dequeue item, false otherwise
    bool dequeue_for(T &popped_item, std::chrono::milliseconds wait_duration) {
        {
            std::unique_lock<std::mutex> lock(m_queue_mtx);
            if (!m_push_cv.wait_for(lock, wait_duration, [this] {
                    return !this->m_data.empty();
                })) {
                return false;
            }
            popped_item = std::move(m_data.front());
            m_data.pop_front();
        }
        m_pop_cv.notify_one();
        return true;
    }

    // blocking dequeue without a timeout.
    void dequeue(T &popped_item) {
        {
            std::unique_lock<std::mutex> lock(m_queue_mtx);
            std::cout << "dequeue waiting..." << std::this_thread::get_id() << std::endl;
            m_push_cv.wait(lock, [this] {
                return !this->m_data.empty(); // 如果为空,那么就一直阻塞,直到被唤醒
            });
            popped_item = std::move(m_data.front());
            m_data.pop_front();
        }
        std::cout << "dequeue notify..." << std::this_thread::get_id() << std::endl;
        m_pop_cv.notify_one();
    }

    size_t overrun_counter() {
        std::unique_lock<std::mutex> lock(m_queue_mtx);
        return m_data.overrun_counter();
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(m_queue_mtx);
        return m_data.size();
    }

    void reset_overrun_counter() {
        std::unique_lock<std::mutex> lock(m_queue_mtx);
        m_data.reset_overrun_counter();
    }

 private:
    std::mutex m_queue_mtx;
    std::condition_variable m_push_cv;
    std::condition_variable m_pop_cv;
    data_container<T> m_data;
};

#endif // !__EASYNET_MSG_QUNUE_H
