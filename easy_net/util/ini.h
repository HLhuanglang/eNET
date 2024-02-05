#ifndef __EASYNET_INI_H
#define __EASYNET_INI_H

#include <map>
#include <string>

const char *const k_whitespace_delimiters = " \t\n\r\f\v";
const char *const k_crlf = "\n";

// 配合ini_datatype_e，对每一行数据进行解析
// 可以存储的是section名，也可以是k-v数据
// 当数据类型是 DATA_SECTION时，section=first
// 当数据类型是 DATA_KEY_VAL时，key=first，val=second
using parse_data_t = std::pair<std::string, std::string>;

using kv_t = std::map<std::string, std::string>;
using ini_containder_t = std::map<std::string, kv_t>; // inifile: section-kv

enum class ini_datatype_e {
    DATA_NONE,
    DATA_COMMENT,
    DATA_SECTION,
    DATA_KEY_VAL,
    DATA_UNKNOWN
};

// 存储ini数据
class inifile {
 public:
    std::string get_val(const char *section, const char *key);
    void set_val(const char *section, const char *key, const char *val);

 public:
    inifile() = default;
    ~inifile() {
        if (m_fp != nullptr) {
            fclose(m_fp);
        }
    };

 public:
    ini_containder_t m_data;
    const char *m_file_name = nullptr;
    FILE *m_fp = nullptr;
};

// 提供操作,全部都是静态方法
class ini {
 public:
    // 加载配置文件,并将数据读取到inifile中
    static bool read(inifile &inifile, const char *path);

    // 将inifile对象中的数据写入到原来的ini文件中
    static bool write(inifile &inifile);

 private:
    static ini_datatype_e _parse_line(std::string line, parse_data_t &data);
    static std::string _covert_inifile_to_string(const inifile &ini);
};

#endif