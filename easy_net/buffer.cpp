#include "buffer.h"
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/uio.h> // readv
#include <unistd.h>
#include <vector>

#include "def.h"

buffer::buffer() : m_writeidx(0), m_readidx(0) {
    m_data.resize(k_default_buffer_size);
}

buffer::buffer(size_t init_size) : m_writeidx(0), m_readidx(0) {
    m_data.resize(init_size);
}

// writer

char *buffer::writeable_start() {
    return _begin() + m_writeidx;
}

const char *buffer::writeable_start() const {
    return _begin() + m_writeidx;
}

void buffer::append(const char *data, size_t len) {
    _ensure_writeable_bytes(len);
    std::copy(data, data + len, writeable_start());
    writer_step(len);
}

void buffer::append(const std::string &str) {
    append(str.c_str(), str.length());
}

size_t buffer::get_writer_idx() const {
    return m_writeidx;
}

void buffer::set_writer_idx(size_t idx) {
    m_writeidx = idx;
}

void buffer::writer_step(size_t step) {
    m_writeidx += step;
}

// reader

char *buffer::readable_start() {
    return _begin() + m_readidx;
}

const char *buffer::readable_start() const {
    return _begin() + m_readidx;
}

size_t buffer::get_reader_idx() const {
    return m_readidx;
}

void buffer::set_reader_idx(size_t idx) {
    m_readidx = idx;
}

void buffer::reader_step(size_t step) {
    m_readidx += step;
}

// 获取大小
size_t buffer::readable_size() const {
    return m_writeidx - m_readidx;
}

size_t buffer::writable_size() const {
    return m_data.size() - m_writeidx;
}

size_t buffer::prependable_size() const {
    return m_readidx;
}

size_t buffer::size() {
    return m_data.size();
}

// 获取原始数据
const std::vector<char> &buffer::get_data() {
    return m_data;
}

// 清空(让readidx_和writeidx_指向k_prepend_size)
void buffer::clear() {
    m_writeidx = 0;
    m_readidx = 0;
    m_data.clear();
}

// 辅助函数
char *buffer::_begin() {
    return &*m_data.begin();
}

const char *buffer::_begin() const {
    return &*m_data.begin();
}

void buffer::_ensure_writeable_bytes(size_t len) {
    if (writable_size() < len) {
        _make_space(len);
    }
}

void buffer::_make_space(size_t len) {
    if (prependable_size() + writable_size() >= len) {
        //整个剩余空间还足够存储,则将现有数据全部向前移动.
        size_t readable = readable_size();
        std::copy(readable_start(), writeable_start(), _begin());
        m_readidx = 0;
        m_writeidx = readable;
    } else {
        //空间不够了,则直接扩容
        m_data.resize(m_writeidx + len);
    }
}