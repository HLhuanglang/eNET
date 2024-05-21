# tcp连接断开的处理

TCP关闭连接：
- 正常关闭
- 异常关闭
    - 连接未被监听的端口
	- 服务端程序崩溃
	- 主机断电
	- tcp连接队列已满
		- 半连接队列满了
		- 全连接队列满了
	- 开启SO_LINGER选项
	- 主动关闭方关闭连接时，接收缓冲区还有未处理的数据
	- 主动关闭方close关闭，但在FIN_WAIT2状态接收到数据
- 半关闭
	- 主动发起TCP半关闭流程
	- 被动处理TCP半关闭流程


## 正常关闭(close)



## 异常关闭



## 半关闭(shutdown)

参考：https://stackoverflow.com/questions/52976152/tcp-when-is-epollhup-generated

About shutdown:
- Doing shutdown(SHUT_WR) sends a FIN and marks the socket with SEND_SHUTDOWN.
- Doing shutdown(SHUT_RD) sends nothing and marks the socket with RCV_SHUTDOWN.
- Receiving a FIN marks the socket with RCV_SHUTDOWN.

And about epoll:
- If the socket is marked with SEND_SHUTDOWN and RCV_SHUTDOWN, poll will return EPOLLHUP.
- If the socket is marked with RCV_SHUTDOWN, poll will return EPOLLRDHUP.
- So the HUP events can be read as:
  - EPOLLRDHUP: you have received FIN or you have called shutdown(SHUT_RD). In any case your reading half-socket is hung, that is, you will read no more data.
  - EPOLLHUP: you have both half-sockets hung. The reading half-socket is just like the previous point, For the sending half-socket you did something like shutdown(SHUT_WR).

To complete a a graceful shutdown I would do:
- Do shutdown(SHUT_WR) to send a FIN and mark the end of sending data.
- Wait for the peer to do the same by polling until you get a EPOLLRDHUP.
- Now you can close the socket with grace.

## FAQ
Q：如何感知tcp是全部断开了还是半关闭状态
A：