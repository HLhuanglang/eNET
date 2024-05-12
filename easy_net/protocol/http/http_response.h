/*
** res一般是服务端发送给客户端的消息,当我们构造了http_responese对象并填充完数据后
** 其实最终需要转换成一坨char数据发送,所以应该有一个http_res_to_buffer的方法。
*/
#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include <string>

#include "http_headers.h"

namespace EasyNet {
class HttpResponse {
 public:
    void SetStatusCode(const std::string &code) { m_status_code = code; }
    void SetStatusCodeMsg(const std::string &msg) { m_status_code_msg = msg; }
    void SetBody(const std::string &body) { m_body = body; }

 public:
    std::string SerializeToString();
    std::string SerializeToString() const;

 public:
    std::string m_version{"HTTP/1.1"};  // http协议版本1.0、1.1
    std::string m_status_code;          // 状态码200、301、404等
    std::string m_status_code_msg;      // 状态码描述信息,例如200的描述信息是ok
    mutable HttpHeaders m_headers;      // key-val
    std::string m_body;                 // body的大小写入Content-Length
};
}  // namespace EasyNet

#endif