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
    x 不使用vector<char>，因为vector本身扩容也是要移动的...
    看了一圈书，发现还是采用std::vector<char>来实现更好.

    实现：
*/
#ifndef __BUFFER_H
#define __BUFFER_H

#include "sigleton.h"
#include <cstddef>
#include <unordered_map>

class buffer {
public:
    buffer(size_t size);

    void pop(size_t size);          //删除buffer中数据,从尾部开始删
    void copy(const buffer* other); //当前buffer不足以存下数据时，需要申请更大内存的块,然后做一个拷贝动作
    void clear();                   //清空数据
    size_t size();

public:
    /*
    数据发送从头开始发，写入则从剩余位置开始写
                     offset_
                     ↓
        [----used----|-----left----]
        ↑                          ↑
        data_                      end_of_data_
    */
    size_t end_of_data_; //大小一定时blocksize_t中的一种
    size_t offset_;
    buffer* next_;
    char* data_;
};

class buffer_pool {
    friend sigleton<buffer_pool>;
    enum blocksize_t
    {
        _4kb   = 4096,
        _8kb   = 8192,
        _16kb  = 16384,
        _32kb  = 32768,
        _64kb  = 65536,
        _128kb = 131072,
        _256kb = 262144,
        _512kb = 524800,
        _1mb   = 1048576,
        _2mb   = 2097512,
        _4mb   = 4194304,
        _8mb   = 8388608
    };

public:
    buffer* get(size_t size);
    void release(buffer* buf);
    void clear_all();                          //清空整个buffer pool
    void clear_by_blocksize(blocksize_t type); //将某个大小的内存块链表全部释放.

public:
    buffer_pool() = default;
    ~buffer_pool() { clear_all(); }

private:
    size_t _align_block_size(size_t size);

private:
    std::mutex mtx_;
    std::unordered_map<size_t, buffer*> pool_map_; //在这个上面的buffer块都是未被使用的
};

class read_buffer {
public:
    int read(int fd);
    const char* data() const;

private:
    buffer* buf_ = nullptr;
};

class write_buffer {
public:
private:
    buffer* buf_ = nullptr;
};
#endif