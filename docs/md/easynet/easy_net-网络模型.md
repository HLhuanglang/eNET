# easy_net网络模型

网络模型分析见[服务端架构设计](./../generic/网络模型设计讨论.md)，这份分析主要是针对tcp协议的，对于udp协议见[udp处理模型](./../generic/关于UDP处理模型.md)

## TCP协议

采用multiReactors模型，一个服务有N个tcpsvr，每个tcpsvr都有一个acceptor和eventloop，使用reuseport监听同一个ip和port，由操作系统决定唤醒某一个阻塞在acceptor的进程上(避免惊群问题)。

目前easynet实现上，每一个tcpsvr都是一个reactor，但是不能使用其他空闲的eventloop，只有框架的上层使用者能够使用，因为所有的线程都是回调同一个接口(onNewConnection、onRecvMsg、...)，可以把计算任务丢给不同的eventloop.

## UDP协议

