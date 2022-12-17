#include "http_response.h"
#include "buffer.h"

http_response::http_response()
{
    version_                   = "HTTP/1.1";
    status_code_               = "200";
    status_code_msg_           = "Ok";
    headers_["Content-Length"] = "14";
    body_                      = "<html>M</html>";
}