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
    timer_id() : timer_(nullptr), seq_(0) {}
    timer_id(timer *timer, size_t seq) : timer_(timer), seq_(seq) {}

private:
    timer *timer_;
    size_t seq_;
};
using timer_id_t = timer_id;

#endif