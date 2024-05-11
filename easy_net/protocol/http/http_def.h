#ifndef __EASYNET_HTTP_H
#define __EASYNET_HTTP_H

#include <functional>

namespace EasyNet {

class HttpRequest;
class HttpResponse;

// 不能修改请求,因此设置为const。此时req只能调用const成员函数
using HttpCallBack = std::function<void(const HttpRequest &req, HttpResponse &rsp)>;
} // namespace EasyNet

#endif // !__EASYNET_HTTP_H
