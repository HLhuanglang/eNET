#include "log.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <cstdarg>
#include <iostream>
#include <thread>

struct __logger {
    unsigned int bufsize_;    //待输出数组的大小
    char *buf_;               //指向待输出数组的指针
    log_level_e log_level_;   //日志级别
    std::string file_path_;   //日志文件路径
    std::string file_name_;   //日志文件名字
    size_t max_logfilesize_;  //日志文件大小
    bool enable_fsync_;       //同步刷新到磁盘
    FILE *fp_;                //文件流指针
    std::timed_mutex mtx_;    //锁
    log_target_e log_target_; //日志输出地
};

log::log() {
    logger_ = new __logger();
}

log::~log() {
    if (logger_) {
        if (logger_->buf_) {
            free(logger_->buf_);
        }
        if (logger_->fp_) {
            fclose(logger_->fp_);
        }
        delete logger_;
    }
}

void log::logger_init() {
    std::call_once(this->once_, [&]() {
        logger_->bufsize_ = DEFAULT_LOG_MAX_BUFSIZE;
        logger_->buf_ = (char *)malloc(logger_->bufsize_);
        logger_->log_level_ = DEFAULT_LOG_LEVEL;
        logger_->file_path_ = DEFAULT_LOG_FILE_PATH;
        logger_->file_name_ = DEFAULT_LOG_FILE_NAME;
        logger_->max_logfilesize_ = DEFAULT_LOG_MAX_FILESIZE;
        logger_->enable_fsync_ = true;
        logger_->fp_ = NULL;
        logger_->log_target_ = DEFAULT_LOG_TARGERT;
    });
}

void log::logger_set_log_target(log_target_e targert) {
    logger_->log_target_ = targert;
}

void log::logger_set_filepath(const std::string &file_path) {
    logger_->file_path_ = file_path;
}

void log::logger_set_logfile_name(const std::string &file_name) {
    logger_->file_name_ = file_name;
}

void log::logger_set_level(log_level_e log_level) {
    logger_->log_level_ = log_level;
}

void log::logger_set_filesize(size_t file_size) {
    logger_->max_logfilesize_ = file_size;
}

void log::logger_enable_fsync(bool flag) {
    if (flag) {
        logger_->enable_fsync_ = true;
    }
    else {
        logger_->enable_fsync_ = false;
    }
}

void log::logger_write(const char *buf, size_t size) {
    std::unique_lock<std::timed_mutex> uqlk(this->logger_->mtx_, std::defer_lock);
    if (uqlk.try_lock_for(std::chrono::seconds(1))) {
        if (logger_->fp_ == NULL) {
            logger_->fp_ = fopen((logger_->file_path_ + logger_->file_name_).c_str(), "ab+");
        }
        fwrite(buf, 1, size, logger_->fp_);
        if (logger_->enable_fsync_) {
            fflush(logger_->fp_);
        }
    }
}

void log::logger_print(log_level_e log_level, const char *fmt, ...) {
    if (log_level < logger_->log_level_) {
        return;
    }

    int year, month, day, hour, min, sec, ms;
#ifdef _WIN32
    SYSTEMTIME tm;
    GetLocalTime(&tm);
    year = tm.wYear;
    month = tm.wMonth;
    day = tm.wDay;
    hour = tm.wHour;
    min = tm.wMinute;
    sec = tm.wSecond;
    ms = tm.wMilliseconds;
#else
    struct timeval tv;
    struct tm *tm = NULL;
    gettimeofday(&tv, NULL);
    time_t tt = tv.tv_sec;
    tm = localtime(&tt);
    year = tm->tm_year + 1900;
    month = tm->tm_mon + 1;
    day = tm->tm_mday;
    hour = tm->tm_hour;
    min = tm->tm_min;
    sec = tm->tm_sec;
    ms = tv.tv_usec / 1000;
#endif
    const char *level = "";
    switch (log_level) {
    case log_level_e::LOG_LEVEL_DEBUG: level = "DEBUG"; break;
    case log_level_e::LOG_LEVEL_INFO: level = "INFO"; break;
    case log_level_e::LOG_LEVEL_WARN: level = "WARN"; break;
    case log_level_e::LOG_LEVEL_ERROR: level = "ERROR"; break;
    case log_level_e::LOG_LEVEL_FATAL: level = "FATAL"; break;
    default: level = "UNKNOW"; break;
    }

    char *buf = logger_->buf_;
    int bufsize = logger_->bufsize_;
    int len = 0;

    //时间
    len = snprintf(buf + len, bufsize - len, "[%04d-%02d-%02d %02d:%02d:%02d.%03d][%s]", year, month, day, hour, min, sec, ms, level);

    va_list ap;
    va_start(ap, fmt);
    len += vsnprintf(buf + len, static_cast<size_t>(bufsize) - len, fmt, ap);
    va_end(ap);

    switch (logger_->log_target_) {
    case log_target_e::FILE: {
        logger_write(buf, len);
        break;
    }
    case log_target_e::TERMINAL: {
        std::cout << buf;
        break;
    }
    case log_target_e::FILE_AND_TERMINAL: {
        std::cout << buf;
        logger_write(buf, len);
        break;
    }
    default: {
        break;
    }
    }
}