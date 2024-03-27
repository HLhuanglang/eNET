# IP格式转换

## 系列一

### 头文件

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
```

### 结构体

```c
typedef uint32_t in_addr_t;

struct in_addr {
   in_addr_t s_addr;
};
```

### 函数

    ┌───────────────────────────────┬───────────────┬────────────────┐
    │Interface                      │ Attribute     │ Value          │
    ├───────────────────────────────┼───────────────┼────────────────┤
    │inet_aton(), inet_addr(),      │ Thread safety │ MT-Safe locale │
    │inet_network(), inet_ntoa()    │               │                │
    ├───────────────────────────────┼───────────────┼────────────────┤
    │inet_makeaddr(), inet_lnaof(), │ Thread safety │ MT-Safe        │
    │inet_netof()                   │               │                │
    └───────────────────────────────┴───────────────┴────────────────┘

```c
#define _BSD_SOURCE
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
   struct in_addr addr;

   if (argc != 2) {
       fprintf(stderr, "%s <dotted-address>\n", argv[0]);
       exit(EXIT_FAILURE);
   }

   if (inet_aton(argv[1], &addr) == 0) {
       fprintf(stderr, "Invalid address\n");
       exit(EXIT_FAILURE);
   }

   printf("%d\n", addr.s_addr);
   exit(EXIT_SUCCESS);
}

//运行 ./main 127.0.0.1
//输出 16777343
```

## 系列二

- inet_ntop：
- inet_pton：

## 系列三

- inet_net_pton
- inet_net_ntop

