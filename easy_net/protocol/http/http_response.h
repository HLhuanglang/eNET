/*
** res一般是服务端发送给客户端的消息,当我们构造了http_responese对象并填充完数据后
** 其实最终需要转换成一坨char数据发送,所以应该有一个http_res_to_buffer的方法。
**
** Connection：keep-alive 是不断开链接.这个也需要处理.
*/
#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include <map>
#include <string>

#include "buffer.h"

class http_response {
public:
public:
    void set_status_code(const std::string &code) { status_code_ = code; }
    void set_status_code_msg(const std::string &msg) { status_code_msg_ = msg; }
    void set_close_connection(bool f) { close_connection_ = f; };

    void set_body(const char *body);

    void default_init(); //默认填充一些值

public:
    static void append_http_to_buf(const http_response &rsp, buffer &buf);

public:
    std::string version_;         // http1.1
    std::string status_code_;     //状态码200、301、404等
    std::string status_code_msg_; //状态码描述信息,例如200的描述信息是ok
    std::map<std::string, std::string> headers_;
    std::string body_;
    //是否关闭
    bool close_connection_;
};

#endif