# EasyNet开发日志

## 2022年9月17日
今天从较高的层面来看怎么设计api，站在一个库的使用者角度。初步的样子：

服务端：
``` cpp
#include "../../net/easy_net.h"

#include <iostream>

int main() {
    event_loop loop;
    tcp_server server(&loop, "127.0.0.1", 8888);

    // 1,设置连接建立回调
    server.set_build_connection_cb([]() {
        // 1,1打印日志
        std::cout << "connection build!" << std::endl;
        // 1,2记录对端信息
    });

    // 2,设置断开连接回调
    server.set_close_connection_cb([]() {
        // 2,1打印日志
        std::cout << "connection close!" << std::endl;
        // 2,2记录对端信息
    });

    // 3,设置消息处理回调
    server.set_recv_msg_cb([&](tcp_connection* conn, buffer* buf) {
        // 3,1 处理数据：todo handle buf
        // 2.2 回复客户端
        std::string ret = "hello,client!";
        conn->send_data(ret.c_str(), ret.size());
    });

    // 4,启动
    server.set_thread_cnt(4);
    server.start();
}
```
客户端：
``` cpp
#include "../../net/easy_net.h"

#include <iostream>

int main() {
    event_loop loop;
    tcp_client client(&loop, "127.0.0.1", 8888);

    // 1,设置连接建立回调
    client.set_build_connection_cb([](tcp_client* cli, void* args) {
        //客户端主动发送数据的地方!
        std::string resp = "hello,server!";
        cli->send_data(resp.c_str(), resp.size());
    });

    // 2,设置连接断开回调
    client.set_close_connection_cb([]() {
        // 2,1打印日志
        std::cout << "connection close!" << std::endl;
        // 2,2记录对端信息
    });

    // 3,设置消息处理回调
    client.set_recv_msg_cb([](tcp_connection* conn, buffer* buf) {
        // 3.1 处理数据：todo handle buf
    });

    // 4,启动
    client.start();
}

/*
    上面这种接口模式,如果client想在任意逻辑中发送消息,那怎么实现？
    不可能每次想发送消息，都得重新创建一个tcp_client，然后设置一大堆操作回调，这样会显得很呆
    而且代码也不好实现.
*/
```

## 2022年9月18日
关于buffer设计这一块的思考。目前的设想是采用内存池分配预设大小的内存块，假设当前申请了一块64kb的内存块，如果还有数据需要读取的话，就得进行扩容了。扩容的过程就是从内存池中换一块更大的内存块，从64变成128kb，然后需要经历一次拷贝操作。如果频繁的出现拷贝，这个拷贝开销不能忽略...

重新阅读了一下muduo网络库，以及陈硕写的书，里面的很多问题和解决思路值得深究，最初阅读的时候，因为并没有动手实践写过socket，所以很多问题的场景和解决手段的理解压根就不明白。准备先好好阅读一下再来继续写代码。

## 2022年9月19日
尝试使用libuv、libevent写点echo、chat、proxy的demo，感受一下api上的设计。

## 2022年9月23日
创建tcp_server后注册的一堆回调，怎么和sub_reactor联系起来呢？

每一个tcp连接都是一个tcp_connection对象，真正的数据收发都在tcp_connection中进行操作。此时如果多个连接都有数据到达，那么recv_msg_cb应该返回哪一个连接的数据？

假设目前是使用http协议来进行通信，客户端主动send_data，客户端自己只要管好自己一个socket就行，发送的数据是一个完整的http包。

此时服务端的某个acceptfd收到了数据，将数据全部读取到read_buf中后触发recv_msg_cb，使用http服务肯定是不会让用户注册这些回调的，要像下面一样：
```cpp

http_server server;

server.on_msg("/hello", [](http_request* req, http_response* res){
    if(req->status_code == 200){
        //todo
    }else{
        res->set_body("json数据");
    }
});

```

完事之后，res肯定会根据acceptfd来找到对应的tcp_connection，然后再调用send_data把数据发给客户端。


## 2022年12月10日

## 2023年1月3日

tcp_client设计

## 2023年1月17日

进行重构时，设计类图这块，由于缺乏设计经验，导致思路混乱不清晰。试着从数据收发的角度来捋顺整个链路。

tcp通信中几件重要的事情：

- 连接建立：tcp三次握手结束，进入ESTABLISH状态可以收发数据了
- 数据接收：假设发送128kb的数据，可能被tcp分成很多次发送。每一次发送完一部分后，read会返回-1,errno=EAGAIN。但是接收到的数据不知道包边界，所以收到一部分数据就要做一次拆包，也就是调用recv_msg通知调用者收到数据了，判断当前包是否完整，如果完整再执行计算逻辑，然后再发送给对端回复。如果不完整，就继续接收数据。
- 数据发送完成：因为发送只是把数据写到了发送缓冲(库提供的)当中，socket的发送缓冲区大小是有限的。所以需要一个回调来告诉用户什么时候是真正的发送完成了。
- 连接断开： 主动断开的一方进入TIME_WAIT状态。


连接建立：需要考虑是客户端还是服务端，二者操作不一样。

服务端: listen--->accept

客户端: connect

目前的模型是主线程负责处理连接，子线程负责处理IO事件。通过eventfd机制和消息队列，主线程创建的acceptfd传递到子线程


目前不知道单线程处理连接请求会不会成为性能瓶颈？


默认情况下，一个电脑的ip+port只能被一个线程bind。这就导致了服务端只能有一个线程来做accept操作，在linux内核3.9版本之后，
可以使用SO_REUSEPORT这个选项，该选项允许多个进程绑定同一个ip+port。有了这个特性，那么每一个子线程都能有自己的acceptor，操作系统会确保只有一个线程的accept会返回。

使用这种特性的话，主线程和子线程之间就无需通过eventfd+消息队列，来传输acceptfd了，可以大大减少编程上的难度。





开始有点get到这种基于回调的设计风格了，也就是基于对象的。因为某一个对象的方法可能是由多个类的成员函数组成的。但是调用类的成员函数方法必须是由这个类的对象。所以这个类的对象要么是其他类的一个指针成员，要么就是方法中传入了这个对象。 就是bind！！！

设置std::function时候，如果是类的成员函数，一定需要传入一个对象


http_client的设计考虑

因为可能在业务逻辑中需要同步请求其他服务，就必须得随时创建client对象，用event_loop有点浪费。

关于http_client的实现，可以考虑如下两种方案：

- 封装libcurl
- 重新封装connect -> send -> recv -> http_parser这个流程

根据编译选项来判断使用哪种模式，--with-curl

并不是所有的linux都安装了curl，所以使用--with-curl选项应该确保有curl库了，编译的时候最好提供指定curl库所在的路径。


应用层协议：文本协议类型、二进制类型

目前做的http是文本协议。read\write只能读写字符，传输二进制可以参考[socket传输类型](./4-socket传输类型.md)

## 2023年1月18日
