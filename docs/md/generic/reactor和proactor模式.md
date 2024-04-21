# 高效的IO处理模式

- 同步IO模型通常用于实现reactor
- 异步IO模型通常用于proactor

## reactor模式

### 概念
Reactor 模型有三个重要的组件：
- 多路复用器：由操作系统提供，在 linux 上一般是 select, poll, epoll 等系统调用。
- 事件分发器：将多路复用器中返回的就绪事件分到对应的处理函数中
- 事件处理器：负责处理特定事件的处理函数

运行流程：
![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20240328150330.png)

### 实现
具体拆分有结构如下：
- Handle句柄：用来标识socket连接或是打开文件；
- Synchronous Event Demultiplexer: 多路复用器。
- Event Handler: 事件处理接口
- Concrete Event HandlerA: 事件处理器，实现应用程序所提供的特定事件处理逻辑；
- Reactor：事件分发器
  - 提供接口，供应用程序注册和删除关注的事件句柄；
  - 有就绪事件到来时，分发事件到具体的事件处理器；
  - 运行事件循环；

![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20240328145523.png)

## proactor模式

![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20240328145557.png)

## 使用同步IO模拟proactor模式

本质上就是在线程中把数据读取完成，然后再通知上层。