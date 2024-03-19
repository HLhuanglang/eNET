#ifndef __EASYNET_DEF_H
#define __EASYNET_DEF_H

#include <functional>
#include <memory>

namespace EasyNet {

class TcpConn;

using tcp_connection_t = std::shared_ptr<TcpConn>;
using CallBack = std::function<void(const tcp_connection_t &)>;
using TimerCallBack = std::function<void()>;

} // namespace EasyNet

#endif // !__EASYNET_DEF_H
