#ifndef __EASYNET_C_ARES_H
#define __EASYNET_C_ARES_H

#include <ares.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

class DNSResolver {
 public:
    DNSResolver() {
        struct ares_options options;

        // 设置操作标志
        int optmask = ARES_OPT_FLAGS;
        options.flags = ARES_FLAG_NOCHECKRESP;
        options.flags |= ARES_FLAG_IGNTC;  // UDP only
        // 设置超时时间
        optmask |= ARES_OPT_TIMEOUT;
        options.timeout = 2;
        // 设置socket状态回调
        optmask |= ARES_OPT_SOCK_STATE_CB;
        options.sock_state_cb = NULL;
        options.sock_state_cb_data = NULL;

        // 执行初始化
        int ret = ares_init_options(&m_channel, &options, optmask);
        if (ret != ARES_SUCCESS) {
            m_err_msg = ares_strerror(ret);
        }
    }

    ~DNSResolver() {
        if (m_channel) {
            ares_destroy(m_channel);
        }
    }

    int lookup(const std::string& domain, int& timeout, void* addr, size_t addr_len) {
        dns_res_t res = {NULL, addr, addr_len};
        ares_gethostbyname(m_channel, domain.c_str(), AF_INET, dns_callback, &res);

        struct timeval last, now;
        gettimeofday(&last, NULL);
        int nfds = 1;
        while (nfds) {
            struct timeval *tvp, tv, store = {timeout / 1000, (timeout % 1000) * 1000};
            tvp = ares_timeout(m_channel, &store, &tv);
            int timeout_ms = tvp->tv_sec * 1000 + tvp->tv_usec / 1000;
            nfds = dns_wait_resolve(m_channel, timeout_ms);
            gettimeofday(&now, NULL);
            timeout -= (now.tv_sec - last.tv_sec) * 1000 + (now.tv_usec - last.tv_usec) / 1000;
            std::cout << timeout << std::endl;
            last = now;
        }

        if (res.error_info) {
            m_err_msg = res.error_info;
            return -1;
        }
        return 0;
    }

    operator bool() const {
        return m_channel;
    }

    const std::string& error_info() const {
        return m_err_msg;
    }

 private:
    DNSResolver(const DNSResolver&);
    DNSResolver& operator=(const DNSResolver&);

    struct dns_res_t {
        const char* error_info;
        void* address;
        size_t len;
    };

    static void dns_callback(void* arg, int status, int timeouts, struct hostent* hptr) {
        dns_res_t& res = *(dns_res_t*)arg;
        if (status != ARES_SUCCESS) {
            res.error_info = ares_strerror(status);
            printf("dns_callback err(%s)\n", res.error_info);
            return;
        }

        if (AF_INET == hptr->h_addrtype) {
            char** pptr = hptr->h_addr_list;
            if (*pptr) {
                memcpy(res.address, *pptr, res.len);
                return;
            }

            res.error_info = "no invalid address get";
            printf("dns_callback err(%s)\n", res.error_info);
        } else {
            res.error_info = "addrtype not supported";
            printf("addrtype(%d) not supported\n", hptr->h_addrtype);
        }
    }

    static int dns_wait_resolve(ares_channel channel_, int timeout_ms) {
        if (timeout_ms < 0) {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
            ares_cancel(channel_);
            return 0;
        }
        int nfds;
        int bitmask;
        ares_socket_t socks[ARES_GETSOCK_MAXNUM];
        struct pollfd pfd[ARES_GETSOCK_MAXNUM];
        int i;
        int num = 0;

        bitmask = ares_getsock(channel_, socks, ARES_GETSOCK_MAXNUM);

        for (i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
            pfd[i].events = 0;
            pfd[i].revents = 0;
            if (ARES_GETSOCK_READABLE(bitmask, i)) {
                pfd[i].fd = socks[i];
                pfd[i].events |= POLLRDNORM | POLLIN;
            }
            if (ARES_GETSOCK_WRITABLE(bitmask, i)) {
                pfd[i].fd = socks[i];
                pfd[i].events |= POLLWRNORM | POLLOUT;
            }
            if (pfd[i].events != 0) {
                num++;
            } else {
                break;
            }
        }

        if (num) {
            nfds = poll(pfd, num, timeout_ms);
        } else {
            nfds = 0;
        }

        if (!nfds) {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
            ares_cancel(channel_);
        } else {
            for (i = 0; i < num; i++) {
                ares_process_fd(channel_,
                                (pfd[i].revents & (POLLRDNORM | POLLIN)) ? pfd[i].fd : ARES_SOCKET_BAD,
                                (pfd[i].revents & (POLLWRNORM | POLLOUT)) ? pfd[i].fd : ARES_SOCKET_BAD);
            }
        }
        return nfds;
    }

    ares_channel m_channel;
    std::string m_err_msg;
};

#endif  // !__EASYNET_C_ARES_H
