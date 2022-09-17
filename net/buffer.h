/*
    如果是基于tcp流式协议，则自定义协议层需要拆包和组包
    如果是基于udp协议，因为udp本身包大小就有限制，udp协议本身是有边界的，所以无需处理拆包和组包
    因此，buffer主要用于tcp协议，用于上层自定义协议的数据拆包和组包

    要求：
    1,大小限制：因为每一个tcp连接都有in_buff和out_buff，大小不可能无限大，否则连接数一多，内存占用就满了.
    2,动态缩放：在设定的最大容量内，可自由伸缩
    3,读取操作：因该返回in_buff的数据起始地址和大小，然后调用用户回调，把数据给上层应用，回调结束后in_buff删除这坨数据.
    避免从in_buff又要拷贝一次到上层应用中.
    4,考虑大文件传输

    注意：
    不使用vector<char>，因为vector本身扩容也是要移动的...

    实现：
*/
#ifndef __BUFFER_H
#define __BUFFER_H

#include "../util/sigleton.h"
#include <cstddef>
#include <unordered_map>

class buffer {
public:
    buffer(size_t size);

    void pop(size_t size);
    void copy(const buffer* other);
    void clear();

public:
    /*
    数据发送从头开始发，写入则从剩余位置开始写
                     offset_
                     ↓
        [----used----|-----left----]
        ↑                          ↑
        data_                      end_of_data_
    */
    size_t end_of_data_;
    size_t offset_;
    buffer* next_;
    char* data_;
};

class buffer_pool {
    friend sigleton<buffer_pool>;

public:
    enum blocksize_t {
        _4kb = 4096,
        _8kb = 8192,
        _16kb = 16384,
        _32kb = 32768,
        _64kb = 65536,
        _128kb = 131072,
        _256kb = 262144,
        _512kb = 524800,
        _1mb = 1048576,
        _2mb = 2097512,
        _4mb = 4194304,
        _8mb = 8388608
    };
    buffer* get(size_t size);

public:
    buffer_pool();

private:
    size_t _align_block_size(size_t size);

private:
    std::mutex mtx_;
    std::unordered_map<size_t, buffer*> pool_map_;
};

class read_buffer {
public:
    int read(int fd);

private:
    buffer* buf_;
};

class write_buffer {};
#endif