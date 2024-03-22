#include "server_thread.h"
#include "event_loop.h"
#include "spdlog/spdlog.h"
#include "tcp_server.h"
#include <mutex>
#include <thread>

using namespace EasyNet;

ServerThread::~ServerThread() {
}

EventLoop *ServerThread::StartServerThread() {
    // 1,创建线程
    m_thread = std::thread([this] {
        threadEntry(this);
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

void ServerThread::threadEntry(ServerThread *self) {
    // 1,创建EventLoop
    EventLoop loop;
    // 2,创建TcpServer
    TcpServer svr(0, m_addr, m_name, true, &loop);

    // 3,运行server
    {
        std::unique_lock<std::mutex> uqlk(m_mtx);
        self->m_loop = &loop;
        self->m_ready = true;
        self->m_cv.notify_all();
    }
    spdlog::debug("ChildServer {} Run", m_name);
    svr.start();
    loop.Loop();

    // 出现错误了
    spdlog::error("ChildServer {} err!", m_name);
    self->m_loop = nullptr;
}