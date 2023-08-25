#include "easy_net/buffer.h"
#include <cstring>
#include <iostream>
#include <string>

// 基础读取
bool buffer_test() {
    buffer buf;
    std::string str = "hello world";
    buf.append(str.c_str(), str.length());
    std::string t(buf.readable_start(), buf.readable_size());
    return t == str;
}

// 触发移动-扩容
bool buffer_append_test() {
    buffer buf(64);                   //初始化buffer存储空间为64字节
    std::string str1 = "hello world"; // 11字节
    std::string str2 = "Holo_wo";     // 7字节
    buf.append(str1);
    for (int i = 0; i < 7; i++) {
        buf.append(str2);
    }
    //此时的数据：[hello world][Holo_wo*7][0000]
    //此时的长度：[11][7*7][4]
    //此时的状态：readidx_=0, writeidx_=11+7*7=60, data_.size()=64

    char *addr = buf.readable_start();

    //读取11字节，此时剩下11(prepend)+4(write)字节, 但可写区域只有4字节不够写入，所以触发移动，调整出15字节可写区域
    std::string t(buf.readable_start(), str1.size());
    buf.reader_step(str1.size());

    buf.append(str2);
    if (std::strlen(addr) == 0 && buf.get_writer_idx() == 49) {
        return false;
    }

    //此时剩余8字节，写入11字节，触发扩容
    buf.append(str1);
    return std::strlen(addr) == 0;
}

int main() {
    if (buffer_test()) {
        std::cout << "buffer_test success" << std::endl;
    } else {
        std::cout << "buffer_test failed" << std::endl;
    }

    if (buffer_append_test()) {
        std::cout << "buffer_append_test success" << std::endl;
    } else {
        std::cout << "buffer_append_test failed" << std::endl;
    }

    return 0;
}