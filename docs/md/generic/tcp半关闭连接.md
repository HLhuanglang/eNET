# tcp半关闭连接

About shutdown:
- Doing shutdown(SHUT_WR) sends a FIN and marks the socket with SEND_SHUTDOWN.
- Doing shutdown(SHUT_RD) sends nothing and marks the socket with RCV_SHUTDOWN.
- Receiving a FIN marks the socket with RCV_SHUTDOWN.

And about epoll:
- If the socket is marked with SEND_SHUTDOWN and RCV_SHUTDOWN, poll will return EPOLLHUP.
- If the socket is marked with RCV_SHUTDOWN, poll will return EPOLLRDHUP.
- So the HUP events can be read as:

To complete a a graceful shutdown I would do:
- Do shutdown(SHUT_WR) to send a FIN and mark the end of sending data.
- Wait for the peer to do the same by polling until you get a EPOLLRDHUP.
- Now you can close the socket with grace.