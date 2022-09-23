#ifndef __SOCKET_H
#define __SOCKET_H

class socket {
private:
    int sock_fd_;

public:
public:
    static void set_noblock(int sock_fd);
};
#endif