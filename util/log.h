#ifndef __LOG_H
#define __LOG_H

#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>

#include "sigleton.h"

// newline
#ifdef __GNUC__
#define __newline "\n"
#define __dir_separtor '/'
#define __dir_sepator_str "/"
#else
#define __newline "\r\n"
#define __dir_separtor '\\'
#define __dir_sepator_str "\\"
#endif

#define __filename (strrchr(__FILE__, __dir_separtor) ? strrchr(__FILE__, __dir_separtor) + 1 : __FILE__)

#define DEFAULT_LOG_FILE_PATH "./"
#define DEFAULT_LOG_FILE_NAME "easynet.log"
#define DEFAULT_LOG_LEVEL log_level_e::LOG_LEVEL_DEBUG
#define DEFAULT_LOG_TARGERT log_target_e::TERMINAL
#define DEFAULT_LOG_MAX_BUFSIZE (1 << 14)  // 16k
#define DEFAULT_LOG_MAX_FILESIZE (1 << 24) // 16M

enum class log_level_e {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
};

enum class log_target_e {
    FILE = 0,         //输出到文件
    TERMINAL,         //输出到终端
    FILE_AND_TERMINAL //文件和终端
};

using logger_t = struct __logger;

class log {
    friend class sigleton<log>;

public:
    void logger_init();
    void logger_print(log_level_e log_level, const char *fmt, ...);
    void logger_set_log_target(log_target_e targert);
    void logger_set_filepath(const std::string &file_path);
    void logger_set_logfile_name(const std::string &file_name);
    void logger_set_level(log_level_e log_level);
    void logger_set_filesize(size_t file_size);
    void logger_write(const char *buf, size_t size);
    void logger_enable_fsync(bool flag);

public:
    log();
    ~log();

private:
    logger_t *logger_;
    std::once_flag once_;
};

/***************************************************************
 * 供用户使用的宏
 **************************************************************/

//日志初始化
#define log_init() sigleton<log>::get_instance()->logger_init()

//设置日志信息输出地,若设置为仅输出到终端,则后续宏不起作用
#define log_set_target(target) sigleton<log>::get_instance()->logger_set_log_target(target)

//设置日志文件存放地点,不设置绝对路径,则存放在当前程序运行路径下
#define log_set_filepath(filepath) sigleton<log>::get_instance()->logger_set_filepath(filepath)

//设置日志级别 [DEBUG,INFO,WARN,ERROR,FATAL]
#define log_set_log_level(level) sigleton<log>::get_instance()->logger_set_level(level)

//设置日志文件的大小,超过限制则清理,默认16MB
#define log_set_filesize(filesize) sigleton<log>::get_instance()->logger_set_filesize(filesize)

// 日志信息
#define log_debug(fmt, ...) \
    sigleton<log>::get_instance()->logger_print(log_level_e::LOG_LEVEL_DEBUG, "[%s:%d][%s]" fmt __newline, __filename, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define log_info(fmt, ...) \
    sigleton<log>::get_instance()->logger_print(log_level_e::LOG_LEVEL_INFO, "[%s:%d][%s]" fmt __newline, __filename, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define log_warn(fmt, ...) \
    sigleton<log>::get_instance()->logger_print(log_level_e::LOG_LEVEL_WARN, "[%s:%d][%s]" fmt __newline, __filename, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define log_error(fmt, ...) \
    sigleton<log>::get_instance()->logger_print(log_level_e::LOG_LEVEL_ERROR, "[%s:%d][%s]" fmt __newline, __filename, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define log_fatal(fmt, ...) \
    sigleton<log>::get_instance()->logger_print(log_level_e::LOG_LEVEL_FATAL, "[%s:%d][%s]" fmt __newline, __filename, __LINE__, __FUNCTION__, ##__VA_ARGS__)

// 别名
#define LOG_DEBUG log_debug
#define LOG_INFO log_info
#define LOG_WARN log_warn
#define LOG_ERROR log_error
#define LOG_FATAL log_fatal

#endif