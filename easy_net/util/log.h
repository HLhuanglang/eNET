#ifndef __EASYNET_LOG_H
#define __EASYNET_LOG_H

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#    define DIR_SEPARATOR '\\'
#    define DIR_SEPARATOR_STR "\\"
#else
#    define DIR_SEPARATOR '/'
#    define DIR_SEPARATOR_STR "/"
#endif

#ifndef __FILENAME__
#    define __FILENAME__ (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#ifdef EASYNET_DEBUG
#    define LOG_DEBUG(fmt, args...) fprintf(stdout, "[D][%s:%d:%s]" fmt "\n", __FILENAME__, __LINE__, __FUNCTION__, ##args)
#    define LOG_INFO(fmt, args...) fprintf(stdout, "[I][%s:%d:%s]" fmt "\n", __FILENAME__, __LINE__, __FUNCTION__, ##args)
#    define LOG_ERROR(fmt, args...) fprintf(stdout, "[E][%s:%d:%s]" fmt "\n", __FILENAME__, __LINE__, __FUNCTION__, ##args)
#    define LOG_FATAL(fmt, args...)                                                              \
        fprintf(stdout, "[F][%s:%d:%s]" fmt "\n", __FILENAME__, __LINE__, __FUNCTION__, ##args); \
        abort()
#else
#    define LOG_DEBUG(fmt, args...)
#    define LOG_INFO(fmt, args...)
#    define LOG_ERROR(fmt, args...)
#    define LOG_FATAL(fmt, args...)
#endif

#endif