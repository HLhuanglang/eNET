# easy_net网络模型

不同的网络模型分析见：[服务端架构设计](./../generic/网络模型设计讨论.md)

easy_net一开始就使用了one loop peer thread模式，只是实现的非常简陋，缺少封装。

主线程负责监听，subreactor服务处理具体业务。主线程loop循环，当epoll_wait返回后，通过sub_reactor_pool[就是线程池] 获取一个子线程，通过eventfd唤醒该子线程。子线程loop返回，然后将主线程返回的acceptfd添加到自己的epoll中进行监听。