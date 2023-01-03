#include "tcp_client.h"
#include "print_debug.h"
#include "util.h"
#include <signal.h>

tcp_client::tcp_client(event_loop* loop, const char* ip, size_t port)
    : loop_(loop)
{
    // if (!util::check_ipv4(ip) || !util::check_port(port)) {
    //     printfd("error format ip or port!\n");
    //     exit(-1);
    // }

    // 1,针对信号做一些处理
    if (::signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGHUP failed!");
    }
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGPIPE failed!");
    }
}