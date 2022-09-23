/*
** res一般是服务端发送给客户端的消息,当我们构造了http_responese对象并填充完数据后
** 其实最终需要转换成一坨char数据发送,所以应该有一个http_res_to_buffer的方法。
*/
#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include <string>

class http_response {
private:
    std::string version_;         // http1.1
    std::string status_code_;     //状态码200、301、404等
    std::string status_code_msg_; //状态码描述信息,例如200的描述信息是ok
};

#endif