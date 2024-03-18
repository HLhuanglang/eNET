#include "io_event.h"

#include "event_loop.h"
#include "log.h"

using namespace EasyNet;

void IOEvent::DispatchEvent() {
    if (m_actual_event & IOEvent_READABLE) {
        ProcessReadEvent();
    }

    if (m_actual_event & IOEvent_WRITEABLE) {
        ProcessWriteEvent();
    }
}

void IOEvent::ProcessReadEvent() {
    // do nothing
}
void IOEvent::ProcessWriteEvent() {
    // do nothing
}

void IOEvent::EnableRead() {
    m_expect_event |= IOEvent_READABLE;
    update_event_status(update_opt_e::ENABLE);
}

void IOEvent::EnableWrite() {
    m_expect_event |= IOEvent_WRITEABLE;
    update_event_status(update_opt_e::ENABLE);
}

void IOEvent::DisableRead() {
    m_expect_event &= ~IOEvent_READABLE;
    update_event_status(update_opt_e::DISABLE);
}

void IOEvent::DisableWrite() {
    m_expect_event &= ~IOEvent_WRITEABLE;
    update_event_status(update_opt_e::DISABLE);
}

void IOEvent::DisableReadAndWrite() {
    m_expect_event = IOEvent_NONE;
    update_event_status(update_opt_e::DISABLE);
}

void IOEvent::RemoveEvent() {
    update_event_status(update_opt_e::REMOVE);
}

void IOEvent::update_event_status(update_opt_e opt) {
    std::unique_ptr<Poller> &Poller = m_ioloop->get_poller();
    // 1,已经添加过监控,那么只能做更新(启动或者不启动)或者删除操作
    // 2,未添加过监控,那么只能做添加操作
    if (m_ioloop->IsRegistered(m_fd)) {
        switch (opt) {
            case update_opt_e::ENABLE:
            case update_opt_e::DISABLE: {
                Poller->ModEvent(this);
                break;
            }
            case update_opt_e::REMOVE: {
                Poller->DelEvent(this);
                m_ioloop->UnRegister(m_fd);
                break;
            }
            default:
                LOG_ERROR("Unkown type: %d", static_cast<int>(opt));
        }

    } else {
        Poller->AddEvent(this);
        m_ioloop->Register(m_fd);
    }
}