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


### 2.3 系统默认动作

SIGINT Ctrl-C终端下产生 终止当前进程
SIGABRT 产生SIGABRT信号 默认终止进程，并产生core(核心转储)文件
SIGALRM 由定时器如alarm(),setitimer()等产生的定时器超时触发的信号 终止当前进程
SIGCHLD 子进程结束后向父进程发送 忽略
SIGBUS 总线错误，即发生了某种内存访问错误 终止当前进程并产生核心转储文件
SIGKILL 必杀信号，收到信号的进程一定结束，不能捕获 终止进程
SIGPIPE 管道断裂，向已关闭的管道写操作 进程终止
SIGIO 使用fcntl注册I/O事件,当管道或者socket上由I/O时产生此信号 终止当前进程
SIGQUIT 在终端下Ctrl-\产生 终止当前进程，并产生core文件
SIGSEGV 对内存无效的访问导致即常见的“段错误” 终止当前进程，并产生core文件
SIGSTOP 必停信号，不能被阻塞，不能被捕捉 停止当前进程
SIGTERM 终止进程的标准信号 终止当前进程


## 3. 为什么进程需要处理信号？



## 4. 网络编程中如何处理信号？
UNIX设计哲学”In UNIX, everything is a file”(其实“Everything is a file descriptor”更加合适),显然signal不在这个everything之列。在多线程程序里处理signal本身就是一个很麻烦的事,如果在遇上eventloop就更加不好处理了。

如果能将signal转化为对一个文件描述符的读写，那么信号处理就方便多了。不少的开源程序(如libevent,libev等)采取的做法是:

1. 初始化一个socketpair或者pipe (一边作为write端，一边作为read端)，将其中的read端添加到epoll里
1. 在signal handler中，将信号值写入socketpair或者pipe 的write一端
1. signal到来时，read端文件描述符上的read事件被触发,epoll_wait返回
1. 处理signal

这种做法被叫做”Self-Pipe Trick”。

在Linux 2.6.27之后，信号处理多了一种选择:signalfd。signalfd会生成一个特殊的文件描述符，发往程序的信号都能从这个文件描述符读取到。有了signalfd，用epoll处理信号就容易多了，muduo网络库正是采取的这种做法