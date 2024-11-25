#include "io_event.h"

#include <string>

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

std::string IOEvent::cover_opt_to_string(update_opt_e opt) {
    std::string type;
    switch (opt) {
        case update_opt_e::DISABLE: {
            type = "DISABLE";
            break;
        }
        case update_opt_e::ENABLE: {
            type = "ENABLE";
            break;
        }
        case update_opt_e::REMOVE:
            type = "REMOVE";
            break;
        default:
            type = "Unkonw Type";
    }
    return type;
}

void IOEvent::update_event_status(update_opt_e opt) {
    std::unique_ptr<Poller> &Poller = m_ioloop->GetPoller();
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
                LOG_ERROR("Unkown type: {}", cover_opt_to_string(opt));
        }
    } else {
        switch (opt) {
            case update_opt_e::ENABLE: {
                Poller->AddEvent(this);
                m_ioloop->Register(m_fd);
                break;
            }
            case update_opt_e::DISABLE:
            case update_opt_e::REMOVE: {
                break;
            }
            default:
                LOG_ERROR("Unkown type: {}", cover_opt_to_string(opt));
        }
    }
}