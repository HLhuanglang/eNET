/// Copyright (c) Holo_wo. 2024. All rights reserved.
///
/// @file io_event.h
/// @brief IO读写事件封装
/// @author Holo_wo
/// @date 2024-02-05

#ifndef __EASYNET_FD_EVENT_H
#define __EASYNET_FD_EVENT_H

#include "non_copyable.h"
#include <vector>

#ifdef __linux__
#    include <sys/poll.h>
#endif

namespace EasyNet {

const constexpr int IOEvent_NONE = 0;                    /* No events registered. */
const constexpr int IOEvent_READABLE = POLLIN | POLLPRI; /* Fire when descriptor is readable. */
const constexpr int IOEvent_WRITEABLE = POLLOUT;         /* Fire when descriptor is writable. */

// 前置声明
class EventLoop;
class IOEvent;

// 可读写的IO事件集合
using active_events_t = std::vector<IOEvent *>;

// IO事件类
class IOEvent : public EasyNet::NonCopyable {
 public:
    // 虚函数，派生类可选择是否重写
    virtual void ProcessReadEvent();
    virtual void ProcessWriteEvent();

 public:
    IOEvent(EventLoop *loop, int fd)
        : m_ioloop(loop),
          m_fd(fd) {
        // nothing todo
    }
    virtual ~IOEvent() = default;

    /// @brief 分发处理IO事件
    void DispatchEvent();

    // 从poller中添加、修改、删除
    void EnableRead();
    void EnableWrite();
    void DisableRead();
    void DisableWrite();
    void DisableReadAndWrite();
    void RemoveEvent();

    int GetFD() const { return m_fd; }
    void SetFD(int fd) { m_fd = fd; }
    int GetExpectEvent() const { return m_expect_event; }
    void SetFiredEvents(int revt) { m_actual_event = revt; }

 private:
    enum class update_opt_e {
        ENABLE,
        DISABLE,
        REMOVE,
    };
    void update_event_status(update_opt_e opt);

 protected:
    EventLoop *m_ioloop;  // 负责处理本描述符的IO线程
    int m_fd;             // 被监控的文件描述符
    int m_expect_event{}; // 用户设置期望监听的事件
    int m_actual_event{}; // Poller返回实际监听得到的事件
};
} // namespace EasyNet

#endif