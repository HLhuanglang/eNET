#ifndef __INI_H
#define __INI_H

#include <map>
#include <string>

const char *const k_whitespace_delimiters = " \t\n\r\f\v";
using parse_data_t = std::pair<std::string, std::string>;
using kv_t = std::map<std::string, std::string>;
using ini_containder_t = std::map<std::string, kv_t>; // ini: section-kv

enum class ini_datatype_e { DATA_NONE, DATA_COMMENT, DATA_SECTION, DATA_KEY_VAL, DATA_UNKNOWN };

class ini {
    friend class ini_handle;

public:
    std::string get_val(const char *section, const char *key);

public:
    ini() = default;
    ~ini() = default;

private:
    ini_containder_t data_;
};

class ini_handle {
public:
    class _ini_parser {
    public:
        ini_datatype_e parse_line(std::string line, parse_data_t &data);
    };

public:
    bool read(ini &data);
    bool write(ini &data) const;
    bool gengerate(ini &data) const;

public:
    explicit ini_handle(const char *file_name);
    ~ini_handle();

private:
    _ini_parser *parser_ = nullptr;
    const char *file_name_ = nullptr;
    FILE *fp_ = nullptr;
};

#endif