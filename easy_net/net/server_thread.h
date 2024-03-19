#ifndef __EASYNET_SERVER_THREAD_H
#define __EASYNET_SERVER_THREAD_H

#include "event_loop.h"
#include "inet_addr.h"
#include "tcp_server.h"
#include <condition_variable>
#include <mutex>
#include <thread>

namespace EasyNet {

class ServerThread {
 public:
    ServerThread(std::string name, const InetAddress &addr) : m_name(name), m_addr(addr) {}
    ~ServerThread();

 public:
    EventLoop *StartServerThread();
    void Join();
    void Detach();

 private:
    void threadEntry(ServerThread *self);

 private:
    EventLoop *m_loop;
    InetAddress m_addr;
    std::thread m_thread;
    std::mutex m_mtx;
    std::string m_name;
    std::condition_variable_any m_cv;

    bool m_ready;
};

} // namespace EasyNet

#endif // !__EASYNET_SERVER_THREAD_H
