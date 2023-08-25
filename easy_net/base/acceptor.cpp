#include "acceptor.h"
#include "tcp_server.h"
#include <asm-generic/errno-base.h>
#include <cerrno>

void acceptor::handle_read() {
    //先调用accept处理连接请求
    struct sockaddr addr;
    socklen_t addr_len = sizeof(sockaddr);
    int acceptfd = ::accept(get_fd(), &addr, &addr_len);
    if (acceptfd < 0) {
        if (errno == EMFILE) {
            m_idle->use(get_fd());
        }
    } else {
        m_server->new_connection(acceptfd, "", "");
    }
}