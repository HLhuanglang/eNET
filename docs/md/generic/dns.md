# dns协议

## 1. 概述

### 1.1 dns协议作用

用于处理ip地址和域名之间的转换

```
                 Local Host                        |  Foreign
                                                   |
    +---------+               +----------+         |  +--------+
    |         | user queries  |          |queries  |  |        |
    |  User   |-------------->|          |---------|->|Foreign |
    | Program |               | Resolver |         |  |  Name  |
    |         |<--------------|          |<--------|--| Server |
    |         | user responses|          |responses|  |        |
    +---------+               +----------+         |  +--------+
                                |     A            |
                cache additions |     | references |
                                V     |            |
                              +----------+         |
                              |  cache   |         |
                              +----------+         |

```

### 1.2 dns协议标准

- [RFC1034](https://www.rfc-editor.org/rfc/rfc1034.html)
- [RFC1035](https://www.rfc-editor.org/rfc/rfc1035.html)

## 2. 如何使用

### 2.1 POSIX接口

`getaddrinfo` 和 `getnameinfo` 是 Linux 下用于处理域名和 IP 地址之间转换的两个函数，它们分别属于 `netdb.h` 头文件

#### 2.1.1 getaddrinfo

`getaddrinfo`：此函数用于将主机名和服务名转换为一个或多个 `sockaddr` 结构。给定一个主机名和一个服务名（例如端口号），`getaddrinfo` 会返回一个指向 `addrinfo` 结构的链表，其中包含了与给定主机名和服务名匹配的所有 IP 地址和端口号。这个函数通常用于客户端程序，以便在连接到服务器时获取服务器的 IP 地址和端口号。具体细节可以参考：[what-does-getaddrinfo-do](https://jameshfisher.com/2018/02/03/what-does-getaddrinfo-do/)

函数原型：

```c
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
```

参数说明：

- `node`：主机名或 IP 地址的字符串。
- `service`：服务名或端口号的字符串。
- `hints`：一个指向 `addrinfo` 结构的指针，用于提供关于如何进行名称解析的附加信息。
- `res`：一个指向 `addrinfo` 结构链表的指针，用于存储解析结果。

#### 2.1.2 getnameinfo

`getnameinfo`：此函数用于将 `sockaddr` 结构转换为主机名和服务名。给定一个 `sockaddr` 结构，`getnameinfo` 会返回与该结构匹配的主机名和服务名。这个函数通常用于服务器程序，以便在接受客户端连接时获取客户端的主机名和服务名。

函数原型：

```c
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags);
```

参数说明：

- `addr`：一个指向 `sockaddr` 结构的指针。
- `addrlen`：`sockaddr` 结构的长度。
- `host`：一个字符数组，用于存储解析后的主机名。
- `hostlen`：`host` 数组的长度。
- `serv`：一个字符数组，用于存储解析后的服务名。
- `servlen`：`serv` 数组的长度。
- `flags`：一个整数，用于指定解析过程中的行为。

### 2.2 c-ares库