#include "socket_opt.h"
#include <asm-generic/socket.h>
#include <cerrno> // errno
#include <cstddef>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h> // readv
#include <unistd.h>  // read

size_t socket_opt::read_fd_to_buf(buffer &buf, int fd, int &err) {
    // 执行read读取数据,对端某次发生的数据会被全部接受下来
    char extrabuf[65536];
    struct iovec vec[2];
    ssize_t n = 0;
    size_t read_size = buf.writable_size();

    vec[0].iov_base = buf.writeable_start();
    vec[0].iov_len = read_size;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    do {
        // 可能被系统调用中断,但是实际并没有调用结束,所以用一层while循环.
        const int iovcnt = (read_size < sizeof extrabuf) ? 2 : 1;
        n = ::readv(fd, vec, iovcnt);
    } while (n == -1 && errno == EINTR);

    if (n < 0) {
        // n=-1, errno= EAGAIN时表示 读缓冲区暂时没数据了,需要用户自己拆包确认数据有没有读全,没读全则继续.
        err = errno;
    } else if (static_cast<size_t>(n) <= read_size) {
        buf.writer_step(read_size);
    } else {
        // 读取的数量超过buf的容量,利用栈上空间.
        buf.set_writer_idx(buf.size());
        buf.append(extrabuf, n - read_size);
    }

    // 返回0表示对端关闭了连接.
    return n;
}

size_t socket_opt::write_buf_to_fd(buffer &buf, int fd) {
    int n = 0;
    do {
        n = static_cast<int>(::write(fd, buf.readable_start(), buf.readable_size()));
    } while (n == -1 && errno == EINTR);

    if (n > 0) {
        buf.reader_step(n); // 因为write可能并不能一次全部发送完,所以需要记录实际发送的数据
    } else if (n == -1 && errno == EAGAIN) {
        return 0;
    }

    return n;
}

bool socket_opt::set_reuseport(int fd) {
    int optval = 1;
    return static_cast<bool>(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) >= 0);
}

bool socket_opt::set_reuseaddr(int fd) {
    int optval = 1;
    return static_cast<bool>(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) >= 0);
}

bool socket_opt::set_noblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }

    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return r >= 0;
}