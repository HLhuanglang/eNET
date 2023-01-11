/*
    如果是基于tcp流式协议，则自定义协议层需要拆包和组包
    如果是基于udp协议，因为udp本身包大小就有限制，udp协议本身是有边界的，所以无需处理拆包和组包
    因此，buffer主要用于tcp协议，用于上层自定义协议的数据拆包和组包

    要求：
    1,大小限制：因为每一个tcp连接都有in_buff和out_buff，大小不可能无限大，否则连接数一多，内存占用就满了.
    2,动态缩放：大小自适应
    3,读取操作：因该返回in_buff的数据起始地址和大小，然后调用用户回调，把数据给上层应用，回调结束后in_buff删除这坨数据，避免从in_buff又要拷贝一次到上层应用中.
    4,考虑大文件传输

    接口：
    buffer是提供给七层协议使用的,buffer接口必须灵活好用.
    buffer只提供最小功能，其余封装由上层使用者来做.
*/
#ifndef __BUFFER_H
#define __BUFFER_H

#include <cstddef>
#include <vector>

class http_request;
class http_response;
class buffer {
public:
    buffer();

public:
    char *write_start();             //数据开始地址
    const char *write_start() const; //数据开始地址
    void append(const char *data,
                size_t len); //填充数据(需要注意迭代器失效问题)

    char *readable_start();
    const char *readble_start() const;

    size_t prependable_size();
    size_t writable_size();
    size_t readable_size();

    size_t size();
    const std::vector<char> &get_data();
    void clear();
    // writer
    size_t get_writer_idx();
    void set_writer_idx(size_t idx);
    void writer_step(size_t step);
    // reader
    size_t get_reader_idx();
    void set_reader_idx(size_t idx);
    void reader_step(size_t step);

private:
    char *_begin();
    const char *_begin() const;
    void _make_space(size_t len);
    void _ensure_writeable_bytes(
        size_t len); //确保有足够空间写入数据,如果不够则扩容

private:
    /*
  数据发送从头开始发，写入则从剩余位置开始写
                          readidx_         writeidx_
                         ↓                ↓
      [---prependable----|----readable----|-----writable----]
      ↑                                                     ↑
      0                                                     data_.size()
  */
    std::vector<char> data_;
    size_t writeidx_;
    size_t readidx_;
};
#endif