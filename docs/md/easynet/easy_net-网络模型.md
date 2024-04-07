# easy_net网络模型

不同的网络模型分析见：[服务端架构设计](./../generic/网络模型设计讨论.md)

easynet：多线程svr模式，一个服务有N个tcpsvr，每个tcpsvr都有一个acceptor和eventloop，使用reuseport监听同一个ip和port，由操作系统决定唤醒某一个阻塞在acceptor的进程上(避免惊群问题)，缺点就是不能使用其他空闲的eventloop。


muduo/tantor：多线程eventloop模式，只有一个tcpsver和acceptor，但是有一个eventloop的线程池。