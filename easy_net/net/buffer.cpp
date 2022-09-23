#include "buffer.h"
#include <cassert>
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

size_t buffer::size() {
    return end_of_data_;
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

void buffer_pool::release(buffer* buf) {
    std::lock_guard<std::mutex> lg(mtx_);
    size_t block_idx = buf->size();
    if (pool_map_.count(block_idx)) {
        buffer* tmp = nullptr;
        tmp = pool_map_[block_idx];
        while (tmp->next_ != nullptr) {
            tmp = tmp->next_;
        }
        tmp->next_ = buf;
    }
    else {
        pool_map_[buf->size()] = buf;
    }
}

void buffer_pool::clear_all() {
    // todo
}

void buffer_pool::clear_by_blocksize(blocksize_t type) {
    // toodo
    if (pool_map_.count(type)) {
    }
}

int read_buffer::read(int fd) {
    // 1,获取当前fd需要读取多少数据（to-fix：如果对方发送数据很大，进行多次发送后，此时无法读取到全部数据）
    int read_total_size = 0;
    if (::ioctl(fd, FIONREAD, &read_total_size) == -1) {
        return -1;
    }

    // 2,获取buf用于存储
    if (!buf_) {
        //直接申请全新的
        buf_ = sigleton<buffer_pool>::get_instance()->get(read_total_size);
    }
    else {
        //旧的,判断剩余空间够不够使用,不够就扩容,然后再拷贝旧的
        size_t left_size = buf_->end_of_data_ - buf_->offset_;
        if (left_size < read_total_size) {
            buffer* old_one = buf_;
            buffer* new_one = sigleton<buffer_pool>::get_instance()->get(read_total_size + left_size);
            new_one->copy(old_one);
            sigleton<buffer_pool>::get_instance()->release(old_one);
            buf_ = new_one;
        }
    }

    // 3,执行read读取数据
    int ret = 0;
    do {
        ret = ::read(fd, buf_->data_ + buf_->offset_, read_total_size);
    } while (ret == -1 && errno == EINTR);

    if (ret > 0) {
        assert(ret == read_total_size);
        buf_->offset_ += read_total_size;
    }

    return ret;
}

const char* read_buffer::data() const {
    return buf_->data_;
}