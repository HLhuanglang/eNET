# epoll

epoll的两种工作模式：
- ET
- LT(默认工作方式)

## ET模式

- 解释
    - ET(edge-triggered)是高速工作方式，只支持no-block-socket。在这种模式下，当描述符从未就绪变为就绪时，内核通过epoll告诉你。然后它会假设你知道文件描述符已经就绪，并且不会再为那个文件描述符发送更多的就绪通知。请注意，如果一直不对这个fd作IO操作(从而导致它再次变成未就绪)，内核不会发送更多的通知(only once).
- 优缺点
    - 优点：每次内核只会通知一次，大大减少了内核资源的浪费，提高效率。
    - 缺点：不能保证数据的完整。不能及时的取出所有的数据。
        - 如果采用循环读，如果对端一直死循环往socket里面写数据，进程就会卡在read上面，导致其他fd处在饥饿状态

## LT模式
- 解释
    - LT(level triggered)是**缺省**的工作方式，并且同时支持block和no-block socket.在这种做法中，内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的fd进行IO操作。如果你不作任何操作，内核还是会继续通知你的，所以，这种模式编程出错误可能性要小一点。传统的select/poll都是这种模型的代表．
- 优缺点
    - 优点：确保调用者能得到完整的数据。如果fd还有数据，epoll_wait就会一直返回。
    - 缺点：频繁的通知，以及状态切换 内核态-->用户态

## read/write在ET和LT模式下的表现

### read

- ET: 只有当对端断开连接或重新发送数据时，read操作才会被触发。这是因为在ET模式下，内核只在描述符从未就绪变为就绪时通知你，之后就假设你知道文件描述符已经就绪，并且不会再为那个文件描述符发送更多的就绪通知
- LT: 只要socket缓冲区中还有数据，read操作就会一直被触发。这是因为在LT模式下，内核会持续告诉你一个文件描述符是否就绪了，如果你不作任何操作，内核还是会继续通知你

### write

- ET：只有当socket缓冲区有足够的空间来接收新数据时，write操作才会被触发。这是因为在ET模式下，内核只在描述符从未就绪变为就绪时通知你
- LT：只要socket缓冲区有空间，write操作就会一直被触发。这是因为在LT模式下，内核会持续告诉你一个文件描述符是否就绪了，如果你不作任何操作，内核还是会继续通知你

## 如何使用

```cpp

...
struct epoll_event ee;
bzero(&ee, sizeof ee);
ee.events = EPOLLIN | EPOLLET;    //关注读事件,并设置成ET模式
ee.data.fd = client_fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ee);
...

```

## epoll的实现原理

一张图总结

![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20230918110905.png)

通过文字来描述一下这个过程：

1. 通过调用 epoll_create() 函数创建并初始化一个 eventpoll 对象。
2. 通过调用 epoll_ctl() 函数把被监听的文件句柄 (如socket句柄) 封装成 epitem 对象并且添加到 eventpoll 对象的红黑树中进行管理。
3. 通过调用 epoll_wait() 函数等待被监听的文件状态发生改变。
4. 当被监听的文件状态发生改变时（如socket接收到数据），会把文件句柄对应 epitem 对象添加到 eventpoll 对象的就绪队列 rdllist 中。并且把就绪队列的文件列表复制到 epoll_wait() 函数的 events 参数中。
5. 唤醒调用 epoll_wait() 函数被阻塞（睡眠）的进程。

## 协议栈如何与epoll通信的

epoll是怎么知道哪个io就绪了呢？我们从ip头可以解析出源ip，目的ip和协议，从tcp头可以解析出源端口和目的端口，此时五元组就凑齐了。socket fd --- <源IP地址, 源端口, 目的IP地址, 目的端口, 协议> 一个fd就是一个五元组，知道了fd，我们就能从红黑树中找到对应的结点。

通知的时机：(服务端)
- **三次握手完成后**：协议栈会往全连接队列中添加一个TCB结点，然后触发一个回调函数，通知到epoll里面有个EPOLLIN事件。
- **接收数据回复ACK之后**：客户端发送一个数据包，服务端协议栈接收后回复ACK，之后触发一个回调函数，通知到epoll里面有个EPOLLIN事件。
- **发送数据收到ACK之后**：每个连接的TCB里面都有一个sendbuf，在对端接收到数据并返回ACK以后，sendbuf就可以将这部分确认接收的数据清空，此时sendbuf里面就有剩余空间，此时触发一个回调函数，通知到epoll里面有个EPOLLOUT事件。
- **接收FIN回复ACK之后**： 客户端close后，服务端接收到FIN报文后后回复对端ACK，此时会调用回调函数，通知到epoll有个EPOLLIN事件(此时read返回0)。
- **接收RST回复ACK之后**：对端发送RST报文，回复ack之后也会触发回调函数，通知epoll有一个EPOLLERR事件。

通知的时机：(客户端)

## ET和LT是如何实现的

从协议栈检测到接收数据，就调用一次回调加入到就绪队列，这就是ET。而LT水平触发，检测到recvbuf里面有数据就调用回调加入就绪队列，如果还有数据就再调用回调加入到rdlist。所以ET和LT就是在使用回调的次数上面的差异。（ET是纯天然的只触发一次，而LT是经过一点点代码修改的）。


## epoll的坑

参考：https://andypan.me/zh-hans/posts/2024/08/23/linux-epoll-with-level-triggering-and-edge-triggering/#%E5%B9%BD%E7%81%B5%E4%BA%8B%E4%BB%B6

### 幽灵事件

问题：
解法：先调用调用epoll_ctl通过EPOLL_CTL_DEL将fd从epoll监听中删除，然后再close

### 饥饿问题

### 事件聚合