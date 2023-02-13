#include "fd_event.h"

#include "event_loop.h"

void fd_event::update_event(epoll_opt_e optflag) {
    std::unique_ptr<poller> &poller = ioloop_->get_poller();
    switch (optflag) {
    case epoll_opt_e::ADD_EVENT: {
        poller->add_fd_event(this);
        break;
    }
    case epoll_opt_e::DEL_EVENT: {
        poller->del_fd_event(this);
        break;
    }
    case epoll_opt_e::MOD_EVENT: {
        poller->mod_fd_event(this);
        break;
    }
    }
}

void fd_event::handle_event() {
    ///
    /// POLLIN     0x0001 普通或优先级带数据可读
    /// POLLRDNORM 0x0040 普通数据可读
    /// POLLRDBAND 0x0080 优先级带数据可读
    /// POLLPRI    0x0002 高优先级数据可读
    ///
    /// POLLOUT    0x0004 可以不阻塞的写普通数据和优先级数据
    /// POLLWRNORM 0x0100 可以不阻塞的写普通数据
    /// POLLWRBAND 0x0200 可以不阻塞的写优先级带数据
    ///
    /// POLLERR    0x0008 发生错误
    /// POLLHUP    0x0010 管道的写端被关闭，读端描述符上接收到这个事件
    /// POLLNVAL   0x0020 描述符不是一个打开的文件
    ///
    /// POLLRDHUP  0x2000 TCP连接被对方关闭，或者对方关闭了写操作
    ///            客户端调用close()正常断开连接，在服务器端会触发一个
    ///            事件。在低于 2.6.17 版本的内核中，这个事件EPOLLIN即0x1
    ///            代表连接可读。然后服务器上层软件read连接，只能读到 EOF
    ///            2.6.17 以后的版本增加了EPOLLRDHUP事件，对端连接断开触发
    ///            的事件会包含 EPOLLIN | EPOLLRDHUP，即 0x2001
    ///
    if (revents_ & (POLLERR | POLLNVAL)) {
        // 这里只记录错误，是否close
        // 需要在read流程确认实际的错误
        handle_error();
    }

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        handle_close();
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        handle_read();
    }

    if (revents_ & POLLOUT) {
        handle_write();
    }
}

void fd_event::handle_error() {
    //do nothing
}
void fd_event::handle_close() {
    //do nothing
}
void fd_event::handle_read() {
    //do nothing
}
void fd_event::handle_write() {
    //do nothing
}

void fd_event::enable_reading(epoll_opt_e optflag) {
    events_ |= k_read_event;
    update_event(optflag);
}

void fd_event::enable_writing(epoll_opt_e optflag) {
    events_ |= k_write_evnet;
    update_event(optflag);
}

void fd_event::disable_reading(epoll_opt_e optflag) {
    events_ &= ~k_read_event;
    update_event(optflag);
}

void fd_event::disable_writing(epoll_opt_e optflag) {
    events_ &= ~k_write_evnet;
    update_event(optflag);
}

void fd_event::disable_all(epoll_opt_e optflag) {
    events_ = k_non_event;
    update_event(optflag);
}