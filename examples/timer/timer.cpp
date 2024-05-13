#include <spdlog/common.h>
#include <unistd.h>

#include "event_loop.h"
#include "log.h"

unsigned long long operator"" _s(unsigned long long s) {
    return s * 1000;
}

unsigned long long operator"" _ms(unsigned long long ms) {
    return ms;
}

int main() {
    EasyNet::LogInit(spdlog::level::level_enum::trace);
    EasyNet::EventLoop loop("timer_loop");

    auto t1 = loop.TimerAfter([]() {
        LOG_DEBUG("TimerAfter 2s");
    },
                              2_s);
    loop.CancelTimer(t1);

    auto t2 = loop.TimerEvery([]() {
        LOG_DEBUG("TimerEvery 2s");
        sleep(3);
    },
                              2_s);

    auto t3 = loop.TimerEvery([]() {
        LOG_DEBUG("TimerEvery 3s");
        sleep(1);
    },
                              3_s);

    loop.Loop();
}