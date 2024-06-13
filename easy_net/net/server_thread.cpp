#include "server_thread.h"

#include <mutex>
#include <thread>

#include "event_loop.h"
#include "tcp_server.h"

using namespace EasyNet;

ServerThread::~ServerThread() {
}

EventLoop *ServerThread::StartServerThread() {
    // 1,创建线程
    m_thread = std::thread([this] {
        threadEntry();
    });

    // 2,等待获取EventLoop
    {
        std::unique_lock<std::mutex> uqlk(m_mtx);
        m_cv.wait(uqlk, [&] {
            return m_ready;
        });
    }
    return m_loop;
}

void ServerThread::Join() {
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void ServerThread::Detach() {
    m_thread.detach();
}

void ServerThread::threadEntry() {
    // 创建TcpServer ps：如果callback要做成成员变量,那么这里就要设置一堆回调,因此使用static来和类绑定而非对象
    TcpServer svr(m_name, 0, m_addr);

    // 3,运行server
    {
        std::unique_lock<std::mutex> uqlk(m_mtx);
        m_loop = svr.GetEventLoop();
        m_ready = true;
        m_cv.notify_all();
    }
    svr.start();
}