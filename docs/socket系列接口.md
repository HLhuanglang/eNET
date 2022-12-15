# socket系列接口

## 一般流程如下

![tcp-sync-queue-and-accept-queue-small](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/tcp-sync-queue-and-accept-queue-small.jpg)

重点需要关注accept、write、read这几个调用，其他的调用没有什么特殊细节。

## accept()函数

**accept函数，操作的是socket()返回的套接字。**

一般accept都是使用非阻塞的，lt模式(新连接没处理，下次还触发EPOLLIN事件)。

正确的accept，需要注意几个问题：

### 阻塞模式accept存在的问题

阻塞模式，也就是socket()创建的套接字是阻塞的。因为accept是从已经完成三次握手的tcp队列中取出一个连接的。假设在accept之前，客户端主动发送RST终止连接，如果监听套接字是阻塞的，那么服务器就会一直阻塞在accept调用上，直到新的连接到来accept才会返回。

所以一般socket()创建监听套接字后，都会设置成非阻塞的。当客户在服务器调用 accept 之前中止某个连接时，accept 调用可以立即返回 -1， 这时源自 Berkeley 的实现会在内核中处理该事件，并不会将该事件通知给 epoll，而其他实现把 errno 设置为 ECONNABORTED 或者 EPROTO 错误，我们应该忽略这两个错误。

### ET模式下accept存在的问题

多个连接同时到达，服务器的 TCP 就绪队列瞬间积累多个就绪连接，由于是边缘触发模式，epoll 只会通知一次，accept 只处理一个连接，导致 TCP 就绪队列中剩下的连接都得不到处理。

解决办法是用 while 循环抱住 accept 调用，处理完 TCP 就绪队列中的所有连接后再退出循环。如何知道是否处理完就绪队列中的所有连接呢？ accept 返回 -1 并且 errno 设置为 EAGAIN 就表示所有连接都处理完。



## write函数

**write/read函数，读取的是accept返回的套接字**。

当acceptfd设置成非阻塞的时候，如果调用read/write返回EAGAIN或EWOULDBLOCK从，表示的意思：

- EAGAIN：再试一次
- EWOULDBLOCK：如果这是一个阻塞socket, 操作将被block
- perror(errno)输出：Resource temporarily unavailable

也就是说，可能read时候读缓冲区没数据或者write时候写缓冲区已经满了

而如果是非阻塞socket, read/write立即返回-1, 同 时errno设置为EAGAIN.

所以, 对于阻塞socket, read/write返回-1代表网络出错了.

但对于非阻塞socket, read/write返回-1不一定网络真的出错了.

可能是Resource temporarily unavailable. 这时你应该再试, 直到Resource available.

 

综上, 对于non-blocking的socket,  正确的读写操作为:

读: 忽略掉errno = EAGAIN的错误, 下次继续读　

写: 忽略掉errno = EAGAIN的错误, 下次继续写　



send_data的时候，先搞一个缓冲区存放(异步的概念)，假设buf=8Mb，此时buf塞满了，就把fd添加到epoll中注册一个写事件。当fd的写缓冲区空了，就会触发一个回调，在这个回调里面再把buf填充到fd的写缓冲区去，因为fd的写缓冲区大小有限，不能写太多，写满了write就会返回-1，因此会多次触发回调。这个回调逻辑里面，当buf真正的发送完成了以后，才把fd的EPOLLOUT事件从epoll中移除，避免一直触发回调去调用write发送，结果嘛buf又是个空的。



## read函数

read主要处理连接建立后的动作。accept返回了套接字后：

- 如果read = -1，errno=EAGAIN，就是说暂时没数据读了，先用这点数据看看满不满足某个协议的解析
- 如果read = 0，就是说明对端已经断开了连接了，这个时候就得把这个acceptfd从epoll中删除了。
- 如果read >0，就是返回读取到的正常字节数。
  - 需要套一个while循环，直到read = -1，errno=EAGAIN才表示本次这波数据接收完成了