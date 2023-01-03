# http客户端接口设计

## api对象创建
- http_client cli("http://ip:port");    方式1：使用完整的服务端地址
- http_client cli("0.0.0.0",12345);     方式2：使用地址+端口