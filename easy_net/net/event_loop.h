#ifndef __EASYNET_EVENT_LOOP_H
#define __EASYNET_EVENT_LOOP_H

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/epoll.h>
#include <unordered_set>
#include <vector>

#include "io_event.h"
#include "non_copyable.h"
#include "notify.h"
#include "poller.h"
#include "timer_miniheap.h"

namespace EasyNet {

using pending_func_t = std::function<void()>;

class EventLoop : public NonCopyable {
 public:
    EventLoop(std::string nameArg);
    ~EventLoop() = default;

 public:
    // loop
    void Loop();

    // 判断当前进程是否是绑定的当前loop
    bool IsThreadInLoop();

    // 提供将函数注入loop的接口
    void RunInLoop(const pending_func_t &cb);
    void QueueInLoop(const pending_func_t &cb);

    // 提供定时任务
    // TODO

    // 当向event_loop中添加了任务后，可立即唤醒event_loop
    void WakeUp() {
        m_notifyer->WakeUp();
    }

    // 退出当前循环
    void Quit();

    // 判断fd是否已添加poller中
    bool IsRegistered(int fd) {
        return m_registered_events.count(fd);
    }
    void Register(int fd) {
        m_registered_events.insert(fd);
    }
    void UnRegister(int fd) {
        m_registered_events.erase(fd);
    }

    std::unique_ptr<Poller> &get_poller() { return m_poller; }

 private:
    void _do_pending_functions();

 private:
    std::string m_name;                              // loop名称
    size_t m_threadid;                               // 当前loop绑定的线程id
    bool m_quit;                                     // 退出标志位
    bool m_looping;                                  // 是否正在循环
    active_events_t m_ready_events;                  // 当前loop上就绪的fd
    std::unique_ptr<Poller> m_poller;                // 管理IO多路复用
    bool m_pending_func;                             // 是否正在执行
    std::vector<pending_func_t> m_pending_func_list; // 待处理的任务列表
    std::unique_ptr<Notify> m_notifyer;              // 使用eventfd或者pipe来唤醒event_loop
    std::unordered_set<int> m_registered_events;     // 当前loop上已注册监听的fd
    std::mutex m_mtx;
    std::unique_ptr<MiniHeapTimer> m_timer_queue; // 定时器队列
};
} // namespace EasyNet

#endif