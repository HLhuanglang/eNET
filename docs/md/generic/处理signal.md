# Linux-信号

## 1. 信号概述

linux下面的信号可以分为**可靠信号**和**不可靠信号**：

- 可靠信号：
- 不可靠信号：信号可能丢失，一旦丢失了，进程并不能知道

使用`man 7 signal`可以查看到信号相关的解释

## 2. 信号的处理方式

### 2.1 忽略信号

大多数信号可以使用这个方式来处理，但是有两种信号不能被忽略（分别是 SIGKILL和SIGSTOP）。因为他们向内核和超级用户提供了进程终止和停止的可靠方法，如果忽略了，那么这个进程就变成了没人能管理的的进程，显然是内核设计者不希望看到的场景

```cpp
signal(SIG, SIG_IGN);    //对某个信号进行忽略
```

### 2.2 捕捉信号

需要告诉内核，用户希望如何处理某一种信号，说白了就是写一个信号处理函数，然后将这个函数告诉内核。当该信号产生时，由内核来调用用户自定义的函数，以此来实现某种信号的处理。

提供的接口有两种：

- 使用signal函数
    - `void (*signal(int sig, void (*func)(int)))(int);`
- 使用sigaction函数
    - `int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact);`
    - oact是用于保存原来的信号处理函数和信号处理的行为

```cpp
#include <csignal>
#include <cstdio>

void signal_handler(int signum) {
    printf("Received signal %d\n", signum);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGQUIT, &sa, nullptr);
    // 在这里注册所有需要处理的信号

    while (true) {
        // 等待信号
    }

    return 0;
}
```

设计信号处理函数的时候需要注意：
1. 一般而言，信号处理函数设计的越简单越好，因为当前代码的执行逻辑被打断，最好尽快恢复到刚才被打断之前的状态。从而避免竞争条件的产生。
2. 在信号处理函数中，建议不要调用printf等与I/O相关的函数。
3. 在信号处理函数中，不要使用任何不可重入的函数


## 3 常见信号源与系统默认动作

```
*1	    SIGHUP	                终止          	终止控制终端或进程

2	    SIGINT	                终止        	键盘产生的中断(Ctrl-C)
3	    SIGQUIT	                dump        	键盘产生的退出
4	    SIGILL	                dump        	非法指令
5	    SIGTRAP	                dump        	debug中断
6	    SIGABRT／SIGIOT	        dump        	异常中止
7	    SIGBUS／SIGEMT	        dump        	总线异常/EMT指令
8	    SIGFPE	                dump        	浮点运算溢出
9	    SIGKILL	                终止        	强制进程终止
10	    SIGUSR1	                终止        	用户信号，进程可自定义用途
11	    SIGSEGV	                dump        	非法内存地址引用
12	    SIGUSR2	                终止        	用户信号，进程可自定义用途

*13	    SIGPIPE	                终止        	向某个没有读取的管道中写入数据

14	    SIGALRM	                终止        	时钟中断(闹钟)
15	    SIGTERM	                终止        	进程终止
16	    SIGSTKFLT	            终止        	协处理器栈错误
17	    SIGCHLD	                忽略        	子进程退出或中断
18	    SIGCONT	                继续        	如进程停止状态则开始运行
19	    SIGSTOP	                停止        	停止进程运行
20	    SIGSTP	                停止        	键盘产生的停止
21	    SIGTTIN	                停止        	后台进程请求输入
22	    SIGTTOU	                停止        	后台进程请求输出

*23	    SIGURG	                忽略        	socket发生紧急情况

24	    SIGXCPU	                dump        	CPU时间限制被打破
25	    SIGXFSZ	                dump        	文件大小限制被打破
26	    SIGVTALRM	            终止        	虚拟定时时钟
27	    SIGPROF	                终止        	profile timer clock
28	    SIGWINCH	            忽略        	窗口尺寸调整
29	    SIGIO/SIGPOLL	        终止        	I/O可用
30	    SIGPWR	                终止        	电源异常
31	    SIGSYS／SYSUNUSED    	dump        	系统调用异常
```



## 3. 为什么进程需要处理信号？



## 4. 网络编程中如何处理信号？
与网络编程有关的信号主要有：`SIGHUP`,`SIGPIPE`和`SIGURG`，由于SIGPIPE会导致进程被终止，所以需要额外关注

### 4.1 SIGPIPE产生的情况

**对端close()后，本端继续发数据，本端将会收到一个RST复位报文提示本端对端已经关闭连接，需重新建立连接。此时再继续往对端发数据，本端系统将会产生SIFPIPE信号，此时本端读取`errno`为`EPIPE`**

### 4.2 如何处理


- send()系统调用的`MSG_NOSIGNAL`可选项可禁止写操作触发`SPGPIPE`信号，所以在这种情况下可以使用send()函数反馈的`errno`值来判断管道或者`socket`连接的对端是否关闭（errno == EPIPE）。
- IO复用也可以用来检测管道和socket连接的读(对)端是否已经关闭，如poll，当读对端关闭时，本(写)端描述符的POLLHUP事件将被触发，当socket连接被对端关闭时，socket上的POLLRDHUP事件将被触发。
- 将signal转化为对一个文件描述符的读写，那么信号处理就方便多了，这种做法被叫做”Self-Pipe Trick”。不少的开源程序(如libevent,libev等)采用这种方式:
  - 初始化一个socketpair或者pipe (一边作为write端，一边作为read端)，将其中的read端添加到epoll里
  - 在signal handler中，将信号值写入socketpair或者pipe 的write一端
  - signal到来时，read端文件描述符上的read事件被触发,epoll_wait返回
  - 处理signal
- 在Linux 2.6.27之后，信号处理多了一种选择:signalfd。signalfd会生成一个特殊的文件描述符，发往程序的信号都能从这个文件描述符读取到。有了signalfd，用epoll处理信号就容易多了，muduo网络库正是采取的这种做法