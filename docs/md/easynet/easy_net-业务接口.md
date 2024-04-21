# API

目前easynet的tcpsvr提供了如下接口：
- set_new_connection_cb
- set_recv_msg_cb
- set_del_connection_cb
- set_write_complete_cb

上层调用者一般来说主要是关注set_recv_msg_cb这个函数，所有的数据首发都在这个入口处进行操作。

由于easynet内部是多线程模式，会存在多个线程同时回调recv_msg，那么就需要业务方来确保自己的接口是线程安全的。