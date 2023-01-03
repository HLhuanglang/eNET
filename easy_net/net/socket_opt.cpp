#include "socket_opt.h"
#include <sys/uio.h> // readv
#include <errno.h>   // errno
#include <unistd.h>  // read

size_t socket_opt::read_fd_to_buf(buffer& buf, int fd, int& err)
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
        buf.set_writer_idx(buf.size());
        buf.append(extrabuf, n - read_size);
    }

    //返回0表示对端关闭了连接.
    return n;
}

size_t socket_opt::write_buf_to_fd(buffer& buf, int fd)
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