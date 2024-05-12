#include <spdlog/common.h>

#include "acceptor.h"
#include "event_loop.h"

unsigned long long operator"" _s(unsigned long long s) {
    return s * 1000;
}

unsigned long long operator"" _ms(unsigned long long ms) {
    return ms;
}

int main() {
    EasyNet::LogInit(spdlog::level::level_enum::trace);
    EasyNet::EventLoop loop("timer_loop");

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