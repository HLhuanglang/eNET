#include "tcp_client.h"
#include <signal.h>

#include "util.h"

tcp_client::tcp_client(event_loop *loop, const char *ip, size_t port)
    : loop_(loop) {

    // 1,针对信号做一些处理
    if (::signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGHUP failed!");
    }
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGPIPE failed!");
    }
}