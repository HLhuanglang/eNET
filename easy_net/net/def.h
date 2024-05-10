#ifndef __EASYNET_DEF_H
#define __EASYNET_DEF_H

#include <functional>
#include <memory>

namespace EasyNet {

class TcpConn;

using tcp_connection_t = std::shared_ptr<TcpConn>;
using CallBack = std::function<void(const tcp_connection_t &)>;
using TimerCallBack = std::function<void()>;

const constexpr int KMaxRetryTimeMS = 5 * 1000;
const constexpr int KInitRetryTimeMS = 500;

} // namespace EasyNet

#endif // !__EASYNET_DEF_H
