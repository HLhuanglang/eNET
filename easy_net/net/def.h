#ifndef __EASYNET_DEF_H
#define __EASYNET_DEF_H

#include <functional>
#include <memory>
#include <thread>

namespace EasyNet {

class TcpConn;

using TcpConnSPtr = std::shared_ptr<TcpConn>;
using EventCallBack = std::function<void(const TcpConnSPtr &)>;
using TimerCallBack = std::function<void()>;

const int KThreadPoolSize = 2 * std::thread::hardware_concurrency();
const constexpr int KDefaultWaitTimeMS = 10;
const constexpr int KMaxRetryTimeMS = 5 * 1000;
const constexpr int KInitRetryTimeMS = 500;

}  // namespace EasyNet

#endif  // !__EASYNET_DEF_H
