# udp服务处理模型

## 关于udp

- 一个UDP socket没有服务器客户端之分，对于服务器来说，一个socket就可以接受所有的客户端数据
- UDP的每个包必然包含了一个完整的包，否则会发送失败。
- 使用四元组来标记通信的双方（单播的情况下）
- udp相比于tcp，在互联网上传输时存在mtu限制的问题，也就是说一个逻辑数据包必须拆分成若干个小于mtu的udp包发送，会有一些问题：
    - udp包乱序以及丢包问题。 业界成熟方案可以考虑用quic、kcp、srt(udt)等
    - 存在应用层分包、组包、排序、重传等机制，并且无粘包问题，需要一个包一次read操作，不好批量收包

## api

udp需要使用到的接口：
- socket()：创建套接字
- bind()：绑定套接字到ip+port
- connect()：⭐连接对端,不同于tcp，对于udp而言，connect是相当于在本地构建起了连接四元组.
- sendto()：发送数据
- recvfrom()：接受数据

对于服务器来说的一般流程：
- fd = socket()
- bind(fd，服务器本机ip，服务器本机port)
- connect(fd，对端ip，对端port)：【这一操作需要看框架模型】
- sendto/recvfrom

对于客户端来说的一般流程：
客户端流程有多种，原因在于源ip+port有多种设置方式。可以自己指定也可以由操作系统决定。虽然使用方式很多，但是归根到底还是对四元组设置的管理。

1. 显示指定ip和端口： socket-->bind-->sendto
    - fd = socket()
    - bind(fd,客户端本机ip,客户端本机port)
    - sendto(fd, 服务器ip,服务器端口,data)
2. 由操作系统决定：socket-->sendto  or socket-->connect-->write/sendto
    - fd = socket()
    - sendto(fd, 服务器ip,服务器端口,data)	
    - connect(fd，服务器ip，服务器端口)
    - write(fd，data) or sendto(fd，null，null，data)

## 模型设计

### 单线程模式

单线程模式下，服务只能一个一个处理请求，由于是无连接的，所以流程大概简化成了如下
```
recvfrom->onRecvMsg->sendto->onWriteComplete
```

### 多线程模式

问题：
1. 服务端如何区分不同的客户端请求

解法：
1. 模拟tcp。当serverfd(逻辑上)有请求时(此时可以拿到对端的ip+port)，新建一个socketfd，该socketfd和serverfd绑定相同的地址和端口(SO_REUSEADDR)，然后调用connect连接客户端，这样子确定唯一的四元组，后续数据收发都在这个socketfd上。
    - 客户端必须固定ip和port，不然四元组会失效。
    - serverfd可能会继续收到对端发过来的数据，因此需要额外的机制来确保当serverfd收到数据时，能将数据转到对应socetfd的poller线程去。




## 参考阅读
- [深入地理解UDP协议并用好它](http://www.52im.net/thread-1024-1-1.html)
- [如何让不可靠的UDP变的可靠?](http://www.52im.net/thread-1293-1-1.html)
- [基于多线程的udp协议accept模型](https://andycong.top/2020/01/05/%E5%9F%BA%E4%BA%8E%E5%A4%9A%E7%BA%BF%E7%A8%8B%E7%9A%84udp%E5%8D%8F%E8%AE%AEaccept%E6%A8%A1%E5%9E%8B/)
- [UDP协议疑难杂症全景解析](https://blog.csdn.net/dog250/article/details/6896949)
- 告知你不为人知的 UDP：疑难杂症和使用
    - [上](https://cloud.tencent.com/developer/article/1004555)
    - [下](https://cloud.tencent.com/developer/article/1004554)