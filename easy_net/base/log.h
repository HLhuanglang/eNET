#ifndef __EASYNET_LOG_H
#define __EASYNET_LOG_H

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace EasyNet {
#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#define DIR_SEPARATOR_STR "\\"
#else
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_STR "/"
#endif

#ifndef __FILENAME__
// #define __FILENAME__  (strrchr(__FILE__, DIR_SEPARATOR) ? strrchr(__FILE__, DIR_SEPARATOR) + 1 : __FILE__)
#define __FILENAME__ (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#define LOG_TRACE(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, fmt, ##__VA_ARGS__)

enum level {
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    err = 4,
    critical = 5,
    off = 6,
    n_levels
};

inline void LogInit(level lv) {
    spdlog::level::level_enum spdlogLevel = static_cast<spdlog::level::level_enum>(lv);
    spdlog::set_level(spdlogLevel);
    spdlog::set_pattern("[%D %H:%M:%S.%e][%L][%t][%s:%# %!] %^%v%$");
}

}  // namespace EasyNet

#endif  // !__EASYNET_LOG_H