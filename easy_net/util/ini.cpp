#include "ini.h"
#include <cstdio>
#include <cstring>

ini_datatype_e ini::parse_line(std::string line, parse_data_t &data)
{
    line.erase(line.find_last_not_of(k_whitespace_delimiters) + 1);
    line.erase(0, line.find_first_not_of(k_whitespace_delimiters));
    data.first.clear();
    data.second.clear();
    if (line.empty()) {
        return ini_datatype_e::DATA_NONE;
    }
    char first_ch = line[0];
    if (first_ch == ';') {
        return ini_datatype_e::DATA_COMMENT;
    }
    if (first_ch == '[') {
        auto comment_idx = line.find_first_of(';');
        if (comment_idx != std::string::npos) {
            line = line.substr(0, comment_idx);
        }
        auto close_bracket_idx = line.find_last_of(']');
        if (close_bracket_idx != line.npos) {
            auto section = line.substr(1, close_bracket_idx - 1);
            data.first = section;
            return ini_datatype_e::DATA_SECTION;
        }
    }
    auto equals_idx = line.find_first_of('=');
    if (equals_idx != line.npos) {
        auto key = line.substr(0, equals_idx);
        auto value = line.substr(equals_idx + 1);
        data.first = key;
        data.second = value;
        return ini_datatype_e::DATA_KEY_VAL;
    }
    return ini_datatype_e::DATA_UNKNOWN;
}

std::string ini::covert_inifile_to_string(const inifile &ini)
{
    std::string data;
    for (auto &n : ini.data_) {
        // section
        data += "[" + n.first + "]" + CRLF;
        if (n.second.size() > 0) {
            // key-val
            for (auto &it : n.second) {
                data += it.first + "=" + it.second + CRLF;
            }
        }
    }
    return data;
}

bool ini::read(inifile &inifile)
{
    char buf[1024] = {0};
    std::string section;
    parse_data_t parse_data;
    while (!feof(inifile.fp_)) {
        fgets(buf, 1024, inifile.fp_);
        size_t len = strlen(buf);
        auto parse_ret = ini::parse_line(buf, parse_data);
        if (parse_ret == ini_datatype_e::DATA_SECTION) {
            section = parse_data.first;
            inifile.data_[section];
        }
        if (parse_ret == ini_datatype_e::DATA_KEY_VAL) {
            inifile.data_[section][parse_data.first] = parse_data.second;
        }
        if (parse_ret == ini_datatype_e::DATA_UNKNOWN) {
            return false;
        }
    }
    return true;
}

bool ini::write(inifile &inifile)
{
    auto data = ini::covert_inifile_to_string(inifile);
    fclose(inifile.fp_);
    inifile.fp_ = fopen(inifile.file_name_, "w");
    if (!inifile.fp_) {
        // perror("fopen error");
        return false;
    }
    fwrite(data.c_str(), 1, data.size(), inifile.fp_);
    return true;
}

bool inifile::load(const char *path)
{
    file_name_ = path;
    fp_ = fopen(file_name_, "r");
    if (!this->fp_) {
        // perror("fopen error");
        return false;
    }
    return true;
}

std::string inifile::get_val(const char *section, const char *key)
{
    return data_[section][key];
}

void inifile::set_val(const char *section, const char *key, const char *val)
{
    data_[section][key] = val;
}