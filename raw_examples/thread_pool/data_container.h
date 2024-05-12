/*
    基于vector实现一个环形buffer
*/

#ifndef __EASYNET_DATA_CONTAINER_H
#define __EASYNET_DATA_CONTAINER_H

#include <cstddef>
#include <vector>

template <typename T>
class data_container {
 private:
    size_t m_max_items = 0;                         // 最大的存储容量
    typename std::vector<T>::size_type m_head = 0;  // 环形buffer-首部
    typename std::vector<T>::size_type m_tail = 0;  // 环形buffer-尾部
    size_t m_overrun_counter = 0;                   // 被覆盖的元素个数
    std::vector<T> m_container;                     // 最底层的数据存储容器

 public:
    data_container() = default;

    explicit data_container(size_t max_items)
        : m_max_items(max_items + 1), m_container(m_max_items) {
        // nothing
    }
    data_container(const data_container &) = default;             // 拷贝构造
    data_container &operator=(const data_container &) = default;  // 赋值操作符重载
    data_container(data_container &&other) noexcept {             // 移动构造
        _copy_moveable(std::move(other));
    }
    data_container &operator=(data_container &&other) noexcept {  // 移动赋值操作符重载
        _copy_moveable(std::move(other));
        return *this;
    }

 public:
    void push_back(T &&item) {
        if (m_max_items > 0) {
            m_container[m_tail] = std::move(item);  // fixme：覆盖写导致信息丢失
            m_tail = (m_tail + 1) % m_max_items;

            if (m_tail == m_head) {
                m_head = (m_head + 1) % m_max_items;
                ++m_overrun_counter;
            }
        }
    }

    const T &front() const {
        return m_container[m_head];
    }

    T &front() {
        return m_container[m_head];
    }

    size_t size() const {
        if (m_tail >= m_head) {
            return m_tail - m_head;
        }
        return m_max_items - (m_head - m_tail);
    }

    const T &at(size_t i) const {
        assert(i < size());
        return m_container[(m_head + i) % m_max_items];
    }

    void pop_front() {
        m_head = (m_head + 1) % m_max_items;
    }

    bool empty() const {
        return m_tail == m_head;
    }

    bool full() const {
        if (m_max_items > 0) {
            return ((m_tail + 1) % m_max_items) == m_head;
        }
        return false;
    }

    size_t overrun_counter() const {
        return m_overrun_counter;
    }

    void reset_overrun_counter() {
        m_overrun_counter = 0;
    }

 private:
    void _copy_moveable(data_container &&other) {
        m_max_items = other.m_max_items;
        m_head = other.m_head;
        m_tail = other.m_tail;
        m_overrun_counter = other.m_overrun_counter;
        m_container = std::move(other.m_container);

        other.m_max_items = 0;
        other.m_head = other.m_tail = 0;
        other.m_overrun_counter = 0;
    }
};

#endif  // !__EASYNET_DATA_CONTAINER_H
