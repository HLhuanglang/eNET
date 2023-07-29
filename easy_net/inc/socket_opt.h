#ifndef __EASYNET_SOCKET_OPT_H
#define __EASYNET_SOCKET_OPT_H

#include <cstddef>

#include "buffer.h"

class socket_opt {
 public:
    //将fd的接受缓冲区中的数据全部读到buf中
    //返回值：
    // n>0：实际收到值
    // n=0：对端关闭了链接
    // n<0：接收错误
    static size_t read_fd_to_buf(buffer &buf, int fd, int &err);

    //将buf中的数据全部写入到fd的发送缓冲区，等待os发送给对端
    //返回值:
    // n>0：实际发送值
    // n=0：由于发送缓冲区满了,只发送了部分数据,还需要再调用
    // n<0：发送错误
    static size_t write_buf_to_fd(buffer &buf, int fd);
};
#endif