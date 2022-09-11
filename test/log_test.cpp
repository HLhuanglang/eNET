#include "../util/log.h"

int main() {
    log_init();
    log_set_target(log_target_e::TERMINAL);
    LOG_DEBUG("hello,world!");
    LOG_INFO("hello,world!");
    LOG_WARN("hello,world!");
    LOG_ERROR("hello,world!");
    LOG_FATAL("hello,world!");
}