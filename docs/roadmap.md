# Roadmap

## v0.0.1
- 学习其他开源的优秀网络库
    - libevent
    - libuv
    - libhv
    - muduo

## v0.0.2
- 网络库本身暂时不做拆包-组包功能,只提供buffer给上层调用者自己解析
- 基本的tcp_server、tcp_client、udp_server、udp_client

## v0.0.3
- 实现一个简单的http_server、http_client

## v0.0.4
- 尝试重构,提高代码质量
- 支持跨平台(linux、mac、windows)

## v0.0.5
- 基于easy_net,做一个简单的项目(web服务器)