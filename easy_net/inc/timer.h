#ifndef __EASYNET_TIMER_H
#define __EASYNET_TIMER_H

#include <cstddef>
#include <functional>

#include "copyable.h"

using timer_cb = std::function<void()>;

class timer {};

// 标记唯一定时器
class timer_id : public copyable {
 public:
    timer_id() : m_timer(nullptr), m_seq(0) {}
    timer_id(timer *timer, size_t seq) : m_timer(timer), m_seq(seq) {}

 private:
    timer *m_timer;
    size_t m_seq;
};
using timer_id_t = timer_id;

#endif