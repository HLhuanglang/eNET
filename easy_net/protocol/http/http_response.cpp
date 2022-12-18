#include "http_response.h"
#include "buffer.h"
#include <string>

void http_response::default_init()
{

    version_                   = "HTTP/1.1";
    status_code_               = "200";
    status_code_msg_           = "Ok";
    headers_["Content-Length"] = "0";
}

void http_response::set_body(const char *body)
{
    body_                      = body;
    headers_["Content-Length"] = std::to_string(body_.size());
}