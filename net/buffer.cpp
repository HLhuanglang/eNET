#include "buffer.h"
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>

buffer::buffer(size_t size) {
    data_ = new char[size];
    end_of_data_ = size;
    offset_ = 0;
}

void buffer::pop(size_t size) {
    offset_ = offset_ - size;
}

void buffer::copy(const buffer* other) {
    ::memmove(data_, other->data_, other->offset_);
    offset_ = other->offset_;
}

void buffer::clear() {
    offset_ = 0;
}

size_t buffer_pool::_align_block_size(size_t size) {
    size_t align = 0;
    if (size <= blocksize_t::_4kb) {
        align = blocksize_t::_4kb;
    }
    else if (size <= blocksize_t::_8kb) {
        align = blocksize_t::_8kb;
    }
    else if (size <= blocksize_t::_16kb) {
        align = blocksize_t::_16kb;
    }
    else if (size <= blocksize_t::_32kb) {
        align = blocksize_t::_32kb;
    }
    else if (size <= blocksize_t::_64kb) {
        align = blocksize_t::_64kb;
    }
    else if (size <= blocksize_t::_128kb) {
        align = blocksize_t::_128kb;
    }
    else if (size <= blocksize_t::_256kb) {
        align = blocksize_t::_256kb;
    }
    else if (size <= blocksize_t::_512kb) {
        align = blocksize_t::_512kb;
    }
    else if (size <= blocksize_t::_1mb) {
        align = blocksize_t::_1mb;
    }
    else if (size <= blocksize_t::_2mb) {
        align = blocksize_t::_2mb;
    }
    else if (size <= blocksize_t::_4mb) {
        align = blocksize_t::_4mb;
    }
    else if (size <= blocksize_t::_8mb) {
        align = blocksize_t::_8mb;
    }
    return align;
}

buffer* buffer_pool::get(size_t size) {
    buffer* ret = nullptr;
    size_t block_type = _align_block_size(size);
    std::lock_guard<std::mutex> lg(mtx_);
    if (pool_map_.count(block_type)) {
        ret = pool_map_[block_type];
        pool_map_[block_type]->next_ = ret->next_;
        return ret;
    }
    else {
        buffer* new_one = new buffer(block_type);
        return new_one;
    }
}

int read_buffer::read(int fd) {
    // 1,获取当前fd需要读取多少数据
    int total_size = 0;
    if (::ioctl(fd, FIONREAD, &total_size) == -1) {
        return -1;
    }

    // 2,获取buf用于存储
    if (!buf_) {
        buf_ = sigleton<buffer_pool>::get_instance()->get(total_size);
    }
    else {
    }

    // 3,执行read读取数据
    int ret = 0;
    do {
        ret = ::read(fd, buf_, total_size);
    } while (ret == -1 && errno == EINTR);
}