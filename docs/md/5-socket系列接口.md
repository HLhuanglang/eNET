参考：

- https://www.cnblogs.com/promise6522/archive/2012/03/03/2377935.html
- https://github.com/balloonwj/CppGuide/blob/master/articles/%E7%BD%91%E7%BB%9C%E7%BC%96%E7%A8%8B/socket%E7%9A%84%E9%98%BB%E5%A1%9E%E6%A8%A1%E5%BC%8F%E5%92%8C%E9%9D%9E%E9%98%BB%E5%A1%9E%E6%A8%A1%E5%BC%8F.md#socket-%E9%9D%9E%E9%98%BB%E5%A1%9E%E6%A8%A1%E5%BC%8F%E4%B8%8B%E7%9A%84-recv-%E8%A1%8C%E4%B8%BA

# socket系列接口

## 一般流程如下

![tcp-sync-queue-and-accept-queue-small](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/tcp-sync-queue-and-accept-queue-small.jpg)

对于socket在阻塞模式和非阻塞模式下，socket系列函数会有不同的表现：
- connect
- accept 
- send/write
- recv/read


## 设置成阻塞和非阻塞模式

linux平台下，默认创建的socket都是阻塞模式的。(所以网络库里面一般都会把socket()和accept()返回的fd设置成非阻塞的)

### 创建时设置成非阻塞

```cpp

int s = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);

```

### 使用fctl或者ioctl设置

```cpp
int oldSocketFlag = fcntl(sockfd, F_GETFL, 0);
int newSocketFlag = oldSocketFlag | O_NONBLOCK;
fcntl(sockfd, F_SETFL,  newSocketFlag);
```


## connect()函数

### 阻塞模式

如下伪代码，当采用阻塞模式的时候，客户端会一直阻塞在connect上，直到成功与服务端建立起连接。connect 函数返回 0 时成功，返回 -1 失败。

```cpp
int main(){
    int listen_fd = socket();
    
    int r = connect(listen_fd);
}
```

在局域网中，这个过程很快可以忽略不记，但是如果服务端在国外，那么connect可能就会阻塞一会，具体多久就要看网络质量了。因此一般客户端都会使用异步connect技术。

### 非阻塞模式

异步connect的流程：
1. 创建socket，并将 socket 设置成非阻塞模式；
2. 调用 connect 函数，此时无论 connect 函数是否连接成功会立即返回；如果返回-1并不表示连接出错，如果此时错误码是EINPROGRESS
3. 接着调用 select 函数，在指定的时间内判断该 socket 是否可写，如果可写说明连接成功，反之则认为连接失败。


## accept()函数

**accept函数，操作的是socket()返回的套接字。**

一般accept都是使用非阻塞的+ epoll lt模式(新连接没处理，下次还触发EPOLLIN事件)。

正确的accept，需要注意几个问题：

### 阻塞模式accept存在的问题

阻塞模式，也就是socket()创建的套接字是阻塞的。因为accept是从已经完成三次握手的tcp队列中取出一个连接的。假设在accept之前，客户端主动发送RST终止连接，如果监听套接字是阻塞的，那么服务器就会一直阻塞在accept调用上，直到新的连接到来accept才会返回。

所以一般socket()创建监听套接字后，都会设置成非阻塞的。当客户在服务器调用 accept 之前中止某个连接时，accept 调用可以立即返回 -1， 这时源自 Berkeley 的实现会在内核中处理该事件，并不会将该事件通知给 epoll，而其他实现把 errno 设置为 ECONNABORTED 或者 EPROTO 错误，我们应该忽略这两个错误。

### 非阻塞+epoll ET模式下accept存在的问题

多个连接同时到达，服务器的 TCP 就绪队列瞬间积累多个就绪连接，由于是边缘触发模式，epoll 只会通知一次，accept 只处理一个连接，导致 TCP 就绪队列中剩下的连接都得不到处理。

解决办法是用 while 循环抱住 accept 调用，处理完 TCP 就绪队列中的所有连接后再退出循环。如何知道是否处理完就绪队列中的所有连接呢？ accept 返回 -1 并且 errno 设置为 EAGAIN 就表示所有连接都处理完。



## send/write和recv/read函数

**读取的是accept返回的套接字**。

send 函数本质上并不是往网络上发送数据，而是将应用层发送缓冲区的数据拷贝到内核缓冲区（下文为了叙述方便，我们以“网卡缓冲区”代指）中去，至于什么时候数据会从网卡缓冲区中真正地发到网络中去要根据 TCP/IP 协议栈的行为来确定，这种行为涉及到一个叫 nagel 算法和 TCP_NODELAY 的 socket 选项。

recv 函数本质上也并不是从网络上收取数据，而只是将内核缓冲区中的数据拷贝到应用程序的缓冲区中，当然拷贝完成以后会将内核缓冲区中该部分数据移除。

可以用下面一张图来描述上述事实：

![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20230831151147.png)


先上结论：

1. recv/read总是在接收缓冲区有数据时立即返回，而不是等到给定的read buffer填满时返回。只有当receive buffer为空时，blocking模式才会等待，而nonblock模式下会立即返回-1（errno = EAGAIN或EWOULDBLOCK）

2. blocking的send/write只有在缓冲区足以放下整个buffer时才返回（与blocking read并不相同）nonblock write则是返回能够放下的字节数，之后调用则返回-1（errno = EAGAIN或EWOULDBLOCK）


### send/write函数

阻塞与非阻塞的区别，主要是看通信的双方的内核缓冲区情况：
- 当内核缓冲区都有空余,那么两个模式没什么区别
- 当双方有一方内核缓冲区满了,那么阻塞模式会阻塞在send/write系统调用上，非阻塞模式则send/write直接返回-1,同时设置errno


对于send/write的返回值意义：
- n>0：
    - 等于给定的字节数：发送成功
    - 小于给定的字节数：写了部分数据,但是被中断信号打断,返回被打断前写入的字节数
- n=0: 不像read/recv,表示对端关闭连接。如果有相应的errno设置，才表示有错误
- n<0(-1)：
    - 阻塞模式：网络出错了
    - 非阻塞模式：
        - errno=EAGAIN：再试一次
        - errno=EWOULDBLOCK：如果这是一个阻塞socket, 操作将被block
        - errno=EINTR, 被打断了
        - perror(errno)输出：Resource temporarily unavailable

### recv/read函数

阻塞与非阻塞的区别，主要是看通信的双方的内核缓冲区情况：
- 当内核缓冲区都有数据,那么两个模式没什么区别
- 当双方有一方内核缓冲区空了,那么阻塞模式会阻塞在recv/read系统调用上,等待数据的到来, 而非阻塞模式则recv/read直接返回-1,同时设置errno

对于recv/read的返回值意义：
- n>0：就是返回读取到的正常字节数。
  - 需要套一个while循环，直到read = -1，errno=EAGAIN才表示本次这波数据接收完成了
- n=0：就是说明对端已经断开了连接了
- n<0(-1):
    - 阻塞模式：网络出错了
    - 非阻塞模式：
        - errno=EAGAIN, 就是说暂时没数据读了，先用这点数据看看满不满足某个协议的解析
        - errno=EWOULDBLOCK
        - errno=EINTR, 被打断了
