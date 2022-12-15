/*
    如果是基于tcp流式协议，则自定义协议层需要拆包和组包
    如果是基于udp协议，因为udp本身包大小就有限制，udp协议本身是有边界的，所以无需处理拆包和组包
    因此，buffer主要用于tcp协议，用于上层自定义协议的数据拆包和组包

    要求：
    1,大小限制：因为每一个tcp连接都有in_buff和out_buff，大小不可能无限大，否则连接数一多，内存占用就满了.
    2,动态缩放：大小自适应
    3,读取操作：因该返回in_buff的数据起始地址和大小，然后调用用户回调，把数据给上层应用，回调结束后in_buff删除这坨数据，避免从in_buff又要拷贝一次到上层应用中.
    4,考虑大文件传输
*/
#ifndef __BUFFER_H
#define __BUFFER_H

#include <cstddef>
#include <vector>

class buffer {
public:
    buffer();

public:
    char* write_start();                       //数据开始地址
    const char* write_start() const;           //数据开始地址
    void append(const char* data, size_t len); //填充数据(需要注意迭代器失效问题)

    size_t prependable_size();
    size_t writable_size();
    size_t readable_size();
    size_t size();
    const std::vector<char>& get_data();
    void clear();
    //writer
    size_t get_writer_idx();
    void set_writer_idx(size_t idx);
    void writer_step(size_t step);
    //reader
    size_t get_reader_idx();
    void set_reader_idx(size_t idx);
    void reader_step(size_t step);

private:
    char* _begin();
    const char* _begin() const;
    void _make_space(size_t len);
    void _ensure_writeable_bytes(size_t len); //确保有足够空间写入数据,如果不够则扩容

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

//===============================================================================
//high level functions
//===============================================================================

//将fd的接受缓冲区中的数据全部读到buf中
//返回值：
//n>0：实际收到值
//n=0：接收缓冲区空了,可能只读取到了部分数据,用户需要先自己拆包看看满不满足协议,不满足则继续等待接收
//n<0：接收错误
size_t read_fd_to_buf(buffer& buf, int fd, int& err);

//将要发送的数据写入buf中.
//
//这个函数主要是为了做异步发送，客户端直接往buf里面写，不让客户端阻塞在write上面
//上层调用的,只要判断buf中有数据就将EPOLLOUT事件添加到epoll,传入回调。在回调中调用write_buf_to_fd
//(上层调用有个小优化逻辑,就是数据必须达到多少量才发送,不然频繁的发送小数据不划算...当然如果只发一点点无法触发发送也是不行的，可以增加一个定时器操作,如果达到多少时间后数据量还是不够,就直接发送)
//当buf写空了以后才把EPOLLOUT事件去除。
void write_buf(buffer& buf, const char* data, size_t len);

//将buf中的数据全部写入到fd的发送缓冲区，等待os发送给对端
//返回值:
//n>0：实际发送值
//n=0：由于发送缓冲区满了,只发送了部分数据,还需要再调用
//n<0：发送错误
size_t write_buf_to_fd(buffer& buf, int fd);

#endif