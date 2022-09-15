# socket系列接口

## 1. read和write

**函数原型**：
- ssize_t read(int fd, void *buf, size_t nbytes);
- ssize_t write(int fd, const void *buf, size_t nbytes);

**参数**：
- int fd：就是Socket本身，这没什么好说的，需要注意的是server_socket与client_socket之间的联系和区分
- void *buf：数据的缓存区，传入的是地址或者指针。
- size_t nbytes：缓存区的长度。

**返回值**
- `>0`：调用正常，返回写入或者读取的字节数
- `=0`：只有read会返回0，表示遇到EOF（对端关闭）
- `=-1`：遇到错误,并且设置了错误码
    - EINTR：中断错误
    - EPIPE：网络错误
    - EAGAIN：非阻塞模式下,不代表错误,只表示fd当前不可读写，下次再操作