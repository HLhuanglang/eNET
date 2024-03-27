# 同步http请求：

- 客户端调用发送请求接口，然后自己阻塞住在请求接口上
- 服务器处理完后返回数据
- 阻塞接口返回，客户端执行后续逻辑

一般为了避免服务器不返回造成客户端阻塞，请求都会设置一个超时时间，当时间到达后服务端还没返回，就认为这次请求失败了。

<img src="https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/同步http请求.png" style="zoom:67%;" />

```c++
void test_http_sync_client(http_client* cli) {
    http_request req;
    req.method = HTTP_POST;
    req.url = "http://127.0.0.1:8080/echo";
    req.headers["Connection"] = "keep-alive";
    req.body = "This is a sync request.";
    req.timeout = 10;
    http_response rsp;
    int ret = cli->send(&req, &rsp);
    if (ret != 0) {
        printf("request failed!\n");
    } else {
        printf("%d %s\r\n", resp.status_code, resp.status_message());
        printf("%s\n", resp.body.c_str());
    }
}
```
这种同步请求, 需要等待发送完成，然后再拿数据做后面的逻辑。http网络访问可能很耗时(tcp连接)，在这段时间里，这个线程就会阻塞在这里，浪费线程资源。如果同时发起多个请求，就会有很多线程被创建浪费...，或是使用任务队列，队列可能暴涨。

# 异步http请求

异步请求的核心在于，调用请求时传入一个回调。当服务端返回后，主要的逻辑在回调中进行处理。这种方式会创建线程，所以需要关注线程的生命周期。

要考虑一个问题：在回调中又执行了发送请求，套娃几层后，会导致回调链路太长了

一般来说业务逻辑上不应该这么写，最多在回调中套一个同步请求。

```cpp
void test_http_async_client(Http_client* cli, int* resp_cnt) {
    http_request* req = new http_request();
    req->method = HTTP_POST;
    req->url = "http://127.0.0.1:8080/echo";
    req->headers["Connection"] = "keep-alive";
    req->body = "This is an async request.";
    req->timeout = 10;
    cli->sendAsync(req, [resp_cnt](const HttpResponsePtr& resp) {
        if (resp == NULL) {
            printf("request failed!\n");
        } else {
            printf("%d %s\r\n", resp->status_code, resp->status_message());
            printf("%s\n", resp->body.c_str());
            //
            //auto ret_json = json_parse(resp->body);
            // 然后根据返回的数据判断是否再请求其他的服务
            // if(ret_json["xxxx"] = xxxx) {
            //     req2->url = "xxxx";
            //     cli->sendAsyc(req2,[resp_cnt](const HttpResponsePtr& resp) {
            //        //再次根据返回的数据，判断后续的业务逻辑    
            //    });
            // }
            //    
        }
        *resp_cnt += 1;
    });
}
```



参考文章

1，https://dongshao.blog.csdn.net/article/details/106357786