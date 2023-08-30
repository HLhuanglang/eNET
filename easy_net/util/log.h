#ifndef __EASYNET_LOG_H
#define __EASYNET_LOG_H

#include <cstdio>

// todo：实现一个异步日志(多生产者，单消费者)
class async_logger {
};

#ifdef DEBUG
#    define LOG_DEBUG(fmt, args...) fprintf(stdout, "[DEBUG] %s:%d " fmt "\n", __FUNCTION__, __LINE__, ##args)
#    define LOG_INFO(fmt, args...) fprintf(stdout, "[INFO] %s:%d" fmt "\n", __FUNCTION__, __LINE__, ##args)
#    define LOG_ERROR(fmt, args...) fprintf(stdout, "[ERROR] %s:%d" fmt "\n", __FUNCTION__, __LINE__, ##args)
#    define LOG_FATAL(fmt, args...) fprintf(stdout, "[FATAL] %s:%d" fmt "\n", __FUNCTION__, __LINE__, ##args)
#elif
#    define LOG_DEBUG(fmt, args...)
#    define LOG_INFO(fmt, args...)
#    define LOG_ERROR(fmt, args...)
#    define LOG_FATAL(fmt, args...)
#endif

#endif