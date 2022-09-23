#include "ini.h"
#include <cstdio>
#include <cstring>

ini_datatype_e ini_handle::_ini_parser::parse_line(std::string line, parse_data_t &data) {
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

ini_handle::ini_handle(const char *file_path) : file_name_(file_path) {
    this->parser_ = new ini_handle::_ini_parser();
}

ini_handle::~ini_handle() {
    if (!this->parser_) {
        delete this->parser_;
    }
    if (!this->fp_) {
        fclose(fp_);
    }
}

bool ini_handle::read(ini &ini) {
    this->file_name_ = file_name_;
    this->fp_ = fopen(file_name_, "r");
    if (!this->fp_) {
        return false;
    }
    char buf[1024] = {0};
    std::string section;
    parse_data_t parse_data;
    while (!feof(this->fp_)) {
        fgets(buf, 1024, this->fp_);
        size_t len = strlen(buf);
        auto parse_ret = this->parser_->parse_line(buf, parse_data);
        if (parse_ret == ini_datatype_e::DATA_SECTION) {
            section = parse_data.first;
            ini.data_[section];
        }
        if (parse_ret == ini_datatype_e::DATA_KEY_VAL) {
            ini.data_[section][parse_data.first] = parse_data.second;
        }
    }
    return true;
}

bool ini_handle::write(ini &data) const {
    // todo
    return true;
}

bool ini_handle::gengerate(ini &data) const {
    // todo
    return true;
}

std::string ini::get_val(const char *section, const char *key) {
    // todo
    return data_[section][key];
}