#include "acceptor.h"
#include "event_loop.h"
#include <spdlog/common.h>

unsigned long long operator"" _s(unsigned long long s) {
    return s * 1000;
}

unsigned long long operator"" _ms(unsigned long long ms) {
    return ms;
}

int main() {
    EasyNet::LogInit(spdlog::level::level_enum::trace);

    EasyNet::EventLoop loop("timer_loop");
    loop.TimerAt([]() {
        LOG_DEBUG("TimerAt 1s");
    },
                 1_s);
    loop.TimerAfter([]() {
        LOG_DEBUG("TimerAfter 2s");
    },
                    2_s);
    loop.TimerEvery([]() {
        LOG_DEBUG("TimerEvery 3s");
    },
                    3_s);

    loop.Loop();
}