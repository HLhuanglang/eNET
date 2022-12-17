#include "buffer.h"
#include <errno.h>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/uio.h> // readv
#include "def.h"
#include <vector>
#include "http_request.h"
#include "http_response.h"

//1
buffer::buffer()
    : writeidx_(0)
    , readidx_(0)
{
    data_.resize(k_default_buffer_size);
}

char* buffer::readable_start()
{
    return _begin() + readidx_;
}

const char* buffer::readble_start() const
{
    return _begin() + readidx_;
}

size_t buffer::readable_size()
{
    return writeidx_ - readidx_;
}

size_t buffer::writable_size()
{
    return data_.size() - writeidx_;
}

size_t buffer::prependable_size()
{
    return readidx_;
}

char* buffer::_begin()
{
    return &*data_.begin();
}

char* buffer::write_start()
{
    return _begin() + writeidx_;
}

const char* buffer::_begin() const
{
    return &*data_.begin();
}

const char* buffer::write_start() const
{
    return _begin() + writeidx_;
}

void buffer::append(const char* data, size_t len)
{
    _ensure_writeable_bytes(len);
    std::copy(data, data + len, write_start());
    writer_step(len);
}

void buffer::_ensure_writeable_bytes(size_t len)
{
    if (writable_size() < len) {
        _make_space(len);
    }
}

void buffer::_make_space(size_t len)
{
    if (prependable_size() + writable_size() >= len + k_prepend_size) {
        //整个剩余空间还足够存储,则将现有数据全部向前移动.
        size_t readable = readable_size();
        std::copy(_begin() + readidx_, _begin() + writeidx_, _begin() + k_prepend_size);
        readidx_  = k_prepend_size;
        writeidx_ = k_prepend_size + readable;
    } else {
        //空间不够了,则直接扩容
        data_.resize(writeidx_ + len);
    }
}

size_t buffer::size()
{
    return data_.size();
}

const std::vector<char>& buffer::get_data()
{
    return data_;
}

void buffer::clear()
{
    writeidx_ = k_prepend_size;
    readidx_  = k_prepend_size;
    data_.clear();
}

size_t buffer::get_reader_idx()
{
    return readidx_;
}

void buffer::set_reader_idx(size_t idx)
{
    readidx_ = idx;
}

void buffer::reader_step(size_t step)
{
    readidx_ += step;
}

size_t buffer::get_writer_idx()
{
    return writeidx_;
}

void buffer::set_writer_idx(size_t idx)
{
    writeidx_ = idx;
}

void buffer::writer_step(size_t step)
{
    writeidx_ += step;
}

size_t read_fd_to_buf(buffer& buf, int fd, int& err)
{
    // 执行read读取数据,对端某次发生的数据会被全部接受下来
    char extrabuf[65536];
    struct iovec vec[2];
    ssize_t n        = 0;
    size_t read_size = buf.writable_size();

    vec[0].iov_base = buf.write_start();
    vec[0].iov_len  = read_size;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len  = sizeof(extrabuf);

    do {
        //可能被系统调用中断,但是实际并没有调用结束,所以用一层while循环.
        const int iovcnt = (read_size < sizeof extrabuf) ? 2 : 1;
        n                = ::readv(fd, vec, iovcnt);
    } while (n == -1 && errno == EINTR);

    if (n < 0) {
        //n=-1, errno= EAGAIN时表示 读缓冲区暂时没数据了,需要用户自己拆包确认数据有没有读全,没读全则继续.
        err = errno;
    } else if (n <= read_size) {
        buf.writer_step(read_size);
    } else {
        //读取的数量超过buf的容量,利用栈上空间.
        //此时writeidx_在buf的末尾,下次写入数据时就会发生扩容.
        buf.set_writer_idx(buf.size());
        buf.append(extrabuf, n - read_size);
    }

    //返回0表示对端关闭了连接.
    return n;
}

size_t write_buf_to_fd(buffer& buf, int fd)
{
    size_t n = 0;
    do {
        n = ::write(fd, buf.readble_start(), buf.readable_size());
    } while (n == -1 && errno == EINTR);

    if (n > 0) {
        buf.reader_step(n);
    } else if (n == -1 && errno == EAGAIN) {
        return 0;
    }

    return n;
}

void append_http_to_buf(const http_response& rsp, buffer& buf)
{
    buf.append(rsp.version_.c_str(), rsp.version_.size());
    buf.append(" ", 1);
    buf.append(rsp.status_code_.c_str(), rsp.status_code_.size());
    buf.append(" ", 1);
    buf.append(rsp.status_code_msg_.c_str(), rsp.status_code_msg_.size());
    buf.append("\r\n", 2);
    for (const auto& it : rsp.headers_) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf.append(str.c_str(), str.size());
    }
    buf.append("\r\n", 2);
    buf.append(rsp.body_.c_str(), rsp.body_.size());
}

void append_http_to_buf(const http_request& req, buffer& buf)
{
    buf.append(req.method_.c_str(), req.method_.size());
    buf.append(" ", 1);
    buf.append(req.url_.c_str(), req.url_.size());
    buf.append(" ", 1);
    buf.append(req.version_.c_str(), req.version_.size());
    buf.append("\r\n", 2);
    for (const auto& it : req.headers_) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf.append(str.c_str(), str.size());
    }
    buf.append("\r\n", 2);
    buf.append(req.body_.c_str(), req.body_.size());
}