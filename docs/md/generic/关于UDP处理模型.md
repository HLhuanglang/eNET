# udp服务处理模型

## api

- 服务端
    - 创建socket，设置服务端本地端口和地址，使用bind绑定
    - recvfrom，接收对端数据，此时就知道了对端的ip+port
    - sendto，根据recvfrom获取的ip+port来给对方发送数据
- 客户端
    - 创建socket，设置本地端口和地址，使用bind绑定
    - 指定服务端ip+port，然后调用sendto发送
    - recvfrom，接收服务端回复的数据以及服务端的ip+port

## 模型设计

### 单线程模式

单线程模式下，服务只能一个一个处理请求，由于是无连接的，所以流程大概简化成了如下
```
recvfrom->onRecvMsg->sendto->onWriteComplete
```

### 多线程模式




## 参考阅读
- [深入地理解UDP协议并用好它](http://www.52im.net/thread-1024-1-1.html)
- [如何让不可靠的UDP变的可靠?](http://www.52im.net/thread-1293-1-1.html)
- [基于多线程的udp协议accept模型](https://andycong.top/2020/01/05/%E5%9F%BA%E4%BA%8E%E5%A4%9A%E7%BA%BF%E7%A8%8B%E7%9A%84udp%E5%8D%8F%E8%AE%AEaccept%E6%A8%A1%E5%9E%8B/)
- [UDP协议疑难杂症全景解析](https://blog.csdn.net/dog250/article/details/6896949)
- 告知你不为人知的 UDP：疑难杂症和使用
    - [上](https://cloud.tencent.com/developer/article/1004555)
    - [下](https://cloud.tencent.com/developer/article/1004554)