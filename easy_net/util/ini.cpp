#include "ini.h"
#include <cstdio>
#include <cstring>

ini_datatype_e ini::_parse_line(std::string line, parse_data_t &data) {
    line.erase(line.find_last_not_of(k_whitespace_delimiters) + 1);
    line.erase(0, line.find_first_not_of(k_whitespace_delimiters));
    data.first.clear();
    data.second.clear();
    if (line.empty()) {
        return ini_datatype_e::DATA_NONE;
    }
    char first_ch = line[0];
    if (first_ch == ';' || first_ch == '#') {
        return ini_datatype_e::DATA_COMMENT;
    }
    if (first_ch == '[') {
        auto comment_idx = line.find_first_of(';');
        if (comment_idx != std::string::npos) {
            line = line.substr(0, comment_idx);
        }
        auto close_bracket_idx = line.find_last_of(']');
        if (close_bracket_idx != std::string::npos) {
            auto section = line.substr(1, close_bracket_idx - 1);
            data.first = section;
            return ini_datatype_e::DATA_SECTION;
        }
    }
    auto equals_idx = line.find_first_of('=');
    if (equals_idx != std::string::npos) {
        auto key = line.substr(0, equals_idx);
        auto value = line.substr(equals_idx + 1);
        data.first = key;
        data.second = value;
        return ini_datatype_e::DATA_KEY_VAL;
    }
    return ini_datatype_e::DATA_UNKNOWN;
}

std::string ini::_covert_inifile_to_string(const inifile &ini) {
    std::string data;
    for (const auto &n : ini.m_data) {
        // section
        data += "[" + n.first + "]" + k_crlf;
        if (!n.second.empty()) {
            // key-val
            for (const auto &it : n.second) {
                data += it.first + "=" + it.second + k_crlf;
            }
        }
    }
    return data;
}

bool ini::write(inifile &inifile) {
    auto data = ini::_covert_inifile_to_string(inifile);
    fclose(inifile.m_fp);
    inifile.m_fp = fopen(inifile.m_file_name, "w");
    if (inifile.m_fp == nullptr) {
        // perror("fopen error");
        return false;
    }
    fwrite(data.c_str(), 1, data.size(), inifile.m_fp);
    return true;
}

bool ini::read(inifile &inifile, const char *path) {
    inifile.m_file_name = path;
    inifile.m_fp = fopen(inifile.m_file_name, "r");
    if (inifile.m_fp != nullptr) {
        char buf[1024] = {0};
        std::string section;
        parse_data_t parse_data;
        while (feof(inifile.m_fp) == 0) {
            fgets(buf, 1024, inifile.m_fp);
            auto parse_ret = ini::_parse_line(buf, parse_data);
            if (parse_ret == ini_datatype_e::DATA_SECTION) {
                section = parse_data.first;
                inifile.m_data[section];
            }
            if (parse_ret == ini_datatype_e::DATA_KEY_VAL) {
                inifile.m_data[section][parse_data.first] = parse_data.second;
            }
            if (parse_ret == ini_datatype_e::DATA_UNKNOWN) {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::string inifile::get_val(const char *section, const char *key) {
    return m_data[section][key];
}

void inifile::set_val(const char *section, const char *key, const char *val) {
    m_data[section][key] = val;
}