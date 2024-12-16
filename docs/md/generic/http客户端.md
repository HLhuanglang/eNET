# http客户端库

关于c++的常用的http客户端，这里做一个列举：
- cpp-httplib：阻塞IO
- cpr：基于libcurl封装

1,简单的同步请求：connect-->send-->recv-->parse-->logic
2,封装到框架之中，提供异步接口
    - 全局只有一个http_client实例
    - auto req = new Request，然后cli->SendRequest(req, callback)
    - 当服务器回包后，触发框架onMsg回调，然后先走http协议栈解析，再回调callback