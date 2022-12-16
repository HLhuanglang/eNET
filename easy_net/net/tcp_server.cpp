#include "tcp_server.h"
#include "print_debug.h"
#include "subreactor_pool.h"
#include "util.h"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

tcp_server::tcp_server(event_loop* loop, const char* ip, size_t port)
    : loop_(loop)
    , sub_reactor_pool_(nullptr)
{
    if (!util::check_ipv4(ip) || !util::check_port(port)) {
        printfd("error format ip or port!\n");
        exit(-1);
    }
    int ret = 0;
    // 1,针对信号做一些处理
    if (::signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGHUP failed!");
    }
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGPIPE failed!");
    }

    // 2,创建监听socket
    socketfd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (socketfd_ < 0) {
        // todo：log
        perror("create socket failed!");
        exit(-1);
    }

    // 3,设置成可重用模式
    int opend = 1;
    ret       = ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR, &opend, sizeof(opend));
    if (ret < 0) {
        perror("setsockopt failed!");
        exit(-1);
    }

    // 4,创建idlefd_
    idlefd_ = ::open("/tmp/easy_net_idle", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
    if (idlefd_ < 0) {
        perror("create idlefd failed!");
        exit(-1);
    }

    // 5,绑定ip和端口
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port        = htons(port);
    ret                  = ::bind(socketfd_, (struct sockaddr*)&addr, sizeof(sockaddr));
    if (ret < 0) {
        // todo:log
        printfd("bind error:%s", strerror(errno));
        exit(-1);
    }

    // 6,进入监听模式
    ::listen(socketfd_, SOMAXCONN);

    // 7,将监听事件以及回调添加到epoll中进行管理。
    loop_ = loop;
    loop_->add_io_event(
        socketfd_, [&](event_loop* loop, int fd, void* args) { this->_do_accept(); }, EPOLLIN, this);
}

void tcp_server::set_recv_msg_cb(recv_msg_cb_f t)
{
    // bug：tcp_server初始化配置,必须等到reactor_pool运行起来.
    auto cnt = sub_reactor_pool_->get_pool_size();
    do {
        auto sub = sub_reactor_pool_->get_sub_reactor();
        sub->set_recv_msg_cb(t);
    } while (cnt--);
}

void tcp_server::set_build_connection_cb(std::function<void()>)
{
    // TODO
}

void tcp_server::set_close_connection_cb(std::function<void()>)
{
    // todo
}

void tcp_server::set_thread_cnt(size_t cnt)
{
    std::lock_guard<std::mutex> lg(mtx_);
    if (sub_reactor_pool_ != nullptr) {
        delete sub_reactor_pool_;
    }
    sub_reactor_pool_ = new subreactor_pool(cnt);
}

void tcp_server::_do_accept()
{
    //先调用accept处理连接请求
    struct sockaddr addr;
    socklen_t addr_len = sizeof(sockaddr);

    bool is_connection_full = false; // linux下进程最多可用1024个fd
    static int cnt          = 0;
    for (;; cnt++) {
        // 1,连接出现了问题
        int acceptfd = ::accept(socketfd_, &addr, &addr_len);
        if (acceptfd == -1) {
            if (errno == EMFILE) {
                is_connection_full = true;
                ::close(idlefd_);
            }
            if (errno == EAGAIN) {
                break;
            }
            if (errno == EINTR) {
                continue;
            }
            perror("accept failed!");
            exit(-1);
        }
        // 2,进程描述符使用完了
        else if (is_connection_full) {
            //处理完连接事件后，直接关闭，然后重新占坑位，确保能一直处理连接事件.
            ::close(acceptfd);
            idlefd_ = ::open("/tmp/easy_net_idle", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
            if (idlefd_ < 0) {
                perror("create idlefd failed!");
                exit(-1);
            }
        }
        // 3,连接正常建立
        else {
            if (sub_reactor_pool_ != nullptr) {
                auto sub = sub_reactor_pool_->get_sub_reactor();
                msg_t msg;
                msg.reactor_id_ = sub_reactor_pool_->get_subreactor_id();
                msg.accept_fd_  = acceptfd;
                msg.msg_type_   = msg_type_t::NEW_CONN;
                sub->notify(msg);
            }
        }
    }
}