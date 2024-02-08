#ifndef __EASYNET_CONFIG_H
#define __EASYNET_CONFIG_H

#include "ini.h"
#include "sigleton.h"
#include <string>
namespace EasyNet {

inline bool InitSDKConf(std::string path = "") {
    if (path.empty()) {
        return false;
    } else {
        return ini::read(*Singleton<inifile>::GetInstance(), path.c_str());
    }
}

// 日志相关配置
#define SDK_LOG_DIR Singleton<inifile>::GetInstance()->get_val("easynet_log", "log_dir")
#define SDK_LOG_NAME Singleton<inifile>::GetInstance()->get_val("easynet_log", "log_name")
#define SDK_LOG_LEVEL Singleton<inifile>::GetInstance()->get_val("easynet_log", "log_level")

// 启动线程相关配置
#define SDK_THREAD_NUM Singleton<inifile>::GetInstance()->get_val("easynet_thread", "thread_num")

} // namespace EasyNet

#endif // !__EASYNET_CONFIG_H
