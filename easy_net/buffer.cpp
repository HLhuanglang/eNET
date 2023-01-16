#include "buffer.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/uio.h> // readv
#include <unistd.h>
#include <vector>

#include "def.h"

// 1
buffer::buffer() : writeidx_(0), readidx_(0) {
    data_.resize(k_default_buffer_size);
}

char *buffer::readable_start() { return _begin() + readidx_; }

const char *buffer::readble_start() const { return _begin() + readidx_; }

size_t buffer::readable_size() { return writeidx_ - readidx_; }

size_t buffer::writable_size() { return data_.size() - writeidx_; }

size_t buffer::prependable_size() { return readidx_; }

char *buffer::_begin() { return &*data_.begin(); }

char *buffer::write_start() { return _begin() + writeidx_; }

const char *buffer::_begin() const { return &*data_.begin(); }

const char *buffer::write_start() const { return _begin() + writeidx_; }

void buffer::append(const char *data, size_t len) {
    _ensure_writeable_bytes(len);
    std::copy(data, data + len, write_start());
    writer_step(len);
}

void buffer::_ensure_writeable_bytes(size_t len) {
    if (writable_size() < len) {
        _make_space(len);
    }
}

void buffer::_make_space(size_t len) {
    if (prependable_size() + writable_size() >= len + k_prepend_size) {
        //整个剩余空间还足够存储,则将现有数据全部向前移动.
        size_t readable = readable_size();
        std::copy(_begin() + readidx_, _begin() + writeidx_,
                  _begin() + k_prepend_size);
        readidx_ = k_prepend_size;
        writeidx_ = k_prepend_size + readable;
    } else {
        //空间不够了,则直接扩容
        data_.resize(writeidx_ + len);
    }
}

size_t buffer::size() { return data_.size(); }

const std::vector<char> &buffer::get_data() { return data_; }

void buffer::clear() {
    writeidx_ = k_prepend_size;
    readidx_ = k_prepend_size;
    data_.clear();
}

size_t buffer::get_reader_idx() { return readidx_; }

void buffer::set_reader_idx(size_t idx) { readidx_ = idx; }

void buffer::reader_step(size_t step) { readidx_ += step; }

size_t buffer::get_writer_idx() { return writeidx_; }

void buffer::set_writer_idx(size_t idx) { writeidx_ = idx; }

void buffer::writer_step(size_t step) { writeidx_ += step; }