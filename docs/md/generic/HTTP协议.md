# http_parser使用

解析tcp收的包：

- Q1：头部+不完整body
- Q2：头部+完整body
- Q3：头部+完整body+其他多余数据

## Q1

- 如果parser没有被重置
    - 第二次解析的时候重新传入整个完整消息，还是只按照conten-length的长度来读取body。
    - 第二次解析传入剩余消息，则只有on_body回调被调用

- 如果parser被重置了
    - 第二次解析重传整个完整消息，则会重新解析 
    - 第二次传入body剩余的部分，直接不解析



# http协议解析

## 协议解析思路
协议解析一般采用状态机的模式来处理，结合http协议的特点，状态机模型其实非常适合。

http协议组成：请求行+请求头+请求体

粗略的设计就完全可以在这三个状态之间进行切换。

## 具体实现
基于easy_net获得的经验总结
```cpp
void http_server::_on_connection(/*tcp_connection& conn*/) {
    // fixme：
    // if (conn->connected()) {
    //     http_context *ctx = new http_context;
    //     conn->setContext((void *)ctx);
    // }
}

int http_server::_on_msg(tcp_connection &conn, buffer &buf) {
    auto sp_ctx = std::make_shared<http_context>();
    http_request req;
    auto parsed_size = sp_ctx->parser_http_context(buf.readble_start(), buf.readable_size(), req); //fixme：这里会从buf拷贝数据...,可否直接使用buf中的数据而避免copy呢?
    if (sp_ctx->is_http_complete()) {
        //不能收到一部分数据就解析一部分数据，并且该部分数据被解析完后还存储在http_server对象中(诸如请求方法、协议版本等信息)
        //因为http_server要处理很多的请求，可能同时有多个连接在收发数据，这样子http_server中保存的解析完成的数据就混乱了
        //
        //上面的想法存在问题
        //
        //两种方案：原理都是状态机
        //
        //方案1：保存解析状态，将状态传入状态机中。tcp_connection中保留一个context变量，用来存储http协议对象，这样子http协议对象就和某条具体的tcp连接关联上了
        // 这样每次收到数据都做一波解析，并且记录当前的状态，然后下一次就能减少前面的重复解析步骤，当前这次解析完成后就在buffer中删掉此次解析的数据
        //
        //方案2：不保存解析状态，状态机每次都要重启。tcp_connection只提供原始数据
        // 这个方案就是每次收到数据都做一次解析，存在重复解析的问题。当解析完成发现是一个完整的http请求后，才返回给调用者，然后才会把数据从buffer中删除。
        //
        //两种方案谁更优呢？
        //方案1，接收到一部分数据就解析一部分，然后释放一部分空间，虽说buffer中的数据被释放了，但是context对象又重新占用了内存呀~
        //方案2看来来多了重复解析以及占用buffer空间的问题，但是http协议的头部其实也没多少字节。问题在于重复解析时，由于使用http_parser会调用一大堆的回调，然后进行string的拷贝操作。
        //完事如果此次接收不完整的话，这一套赋值操作就白费了。
        //
        //还是采用方案1比较好点？自己实现一套http协议的解析。
        _on_request(conn, req);
    } else {
        //收包不完整,继续收
    }
    return static_cast<int>(parsed_size);
}
```


HTTP 协议，全称为超文本传输协议（HyperText Transfer Protocol），是用于从万维网（WWW:World Wide Web ）服务器传输超文本到本地浏览器的传送协议。

HTTP 1.0 和 HTTP 1.1 是 HTTP 协议的两大版本，它们之间存在一些区别。以下是 HTTP 1.0 和 HTTP 1.1 的主要区别：

1. **持久连接（Persistent Connections）**：
   - HTTP 1.0 默认使用短连接，即每次请求/响应后，TCP 连接会被关闭。这可能导致短时间内大量创建和关闭 TCP 连接，浪费资源并降低性能。
   - HTTP 1.1 引入了持久连接的概念，即在一个 TCP 连接上可以发送多个 HTTP 请求和响应的消息。这大大减少了建立和关闭连接的开销，提高了性能。客户端和服务器可以通过设置 `Connection: keep-alive`头来启用持久连接。

2. **管道化（Pipelining）**：
   - HTTP 1.0 不支持管道化，即客户端必须等待前一个请求的响应才能发送下一个请求。这可能导致队头阻塞问题，从而降低了性能。
   - HTTP 1.1 支持管道化，允许客户端在不等待响应的情况下连续发送请求。这样可以更充分地利用网络带宽，但服务器端需要能够正确处理这种并发请求。

3. **分块传输编码（Chunked Transfer Encoding）**：
   - HTTP 1.0 中，服务器在发送响应时，必须先知道响应体的全部大小，并将其放在 `Content-Length` 头中。这在某些情况下可能造成不便，例如当服务器生成动态内容时。
   - HTTP 1.1 引入了分块传输编码机制，允许服务器逐个发送响应体中的数据块，并在每个数据块前加上其大小。这样，服务器可以在不知道响应体总大小的情况下开始发送响应。

4. **缓存控制**：
   - HTTP 1.1 引入了一些新的缓存控制机制，如 `Cache-Control` 和 `ETag` 头，以提供更精细的缓存控制和更好的性能优化。

5. **新增方法**：
   - HTTP 1.1 新增了一些 HTTP 方法，如 `OPTIONS`（获取服务器支持的 HTTP 方法）、`TRACE`（回显服务器收到的请求）和 `CONNECT`（用于代理服务器与隧道连接）。

总之，HTTP 1.1 相对于 HTTP 1.0 进行了许多改进，包括持久连接、管道化、分块传输编码、缓存控制和新增方法等，以提高性能和扩展性。现在大多数网站都已采用 HTTP 1.1 或更高版本的协议。