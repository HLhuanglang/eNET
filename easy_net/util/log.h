#ifndef __EASYNET_LOG_H
#define __EASYNET_LOG_H

#include <cstdio>

// todo：实现一个异步日志(多生产者，单消费者)
class async_logger {
};

#ifdef DEBUG
#    define LOG_DEBUG(fmt, args...) fprintf(stdout, "[DEBUG]" fmt "\n", ##args)
#    define LOG_INFO(fmt, args...) fprintf(stdout, "[INFO]" fmt "\n", ##args)
#    define LOG_ERROR(fmt, args...) fprintf(stdout, "[ERROR]" fmt "\n", ##args)
#    define LOG_FATAL(fmt, args...) fprintf(stdout, "[FATAL]" fmt "\n", ##args)
#elif
#    define LOG_DEBUG(fmt, args...) fprintf(stdout, "[DEBUG]" fmt "\n", ##args)
#    define LOG_INFO(fmt, args...) fprintf(stdout, "[INFO]" fmt "\n", ##args)
#    define LOG_ERROR(fmt, args...) fprintf(stdout, "[ERROR]" fmt "\n", ##args)
#    define LOG_FATAL(fmt, args...) fprintf(stdout, "[FATAL]" fmt "\n", ##args)
#endif

#endif