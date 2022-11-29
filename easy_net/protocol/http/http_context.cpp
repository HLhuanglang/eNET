#include "http_context.h"
#include "http_parser.h"

static int on_url(http_parser* parser, const char* at, size_t length);
static int on_status(http_parser* parser, const char* at, size_t length);
static int on_header_field(http_parser* parser, const char* at, size_t length);
static int on_header_value(http_parser* parser, const char* at, size_t length);
static int on_body(http_parser* parser, const char* at, size_t length);
static int on_message_begin(http_parser* parser);
static int on_headers_complete(http_parser* parser);
static int on_message_complete(http_parser* parser);
static int on_chunk_header(http_parser* parser);
static int on_chunk_complete(http_parser* parser);

http_parser_settings http_context::parser_settings_ = {on_message_begin, on_url, on_status, on_header_field, on_header_value,
                                                       on_headers_complete, on_body, on_message_complete, on_chunk_header, on_chunk_complete};

http_context::http_context()
{
    http_parser_init(&parser_, http_parser_type::HTTP_REQUEST);
    parser_.data = this;
}

size_t http_context::parser_http_context(const char* data, size_t len, http_request& req)
{
    req_ = &req;
    return http_parser_execute(&parser_, &parser_settings_, data, len);
}

void http_context::_handle_header()
{
    if (header_filed_.size() != 0 && header_value_.size() != 0) {
        req_->headers_[header_filed_] = header_value_;
        header_filed_.clear();
        header_value_.clear();
    }
}

int on_message_begin(http_parser* parser)
{
    // todo
    return 0;
}

int on_url(http_parser* parser, const char* at, size_t length)
{
    http_context* ctx = (http_context*)parser->data;
    ctx->req_->url_.assign(at, length);
    return 0;
}

int on_status(http_parser* parser, const char* at, size_t length)
{
    return 0;
}

int on_header_field(http_parser* parser, const char* at, size_t length)
{
    http_context* ctx = (http_context*)parser->data;
    ctx->header_filed_.assign(at, length);
    return 0;
}

int on_header_value(http_parser* parser, const char* at, size_t length)
{
    http_context* ctx = (http_context*)parser->data;
    ctx->header_value_.assign(at, length);
    ctx->_handle_header();
    return 0;
}

int on_headers_complete(http_parser* parser)
{
    // todo
    return 0;
}

int on_body(http_parser* parser, const char* at, size_t length)
{
    http_context* ctx = (http_context*)parser->data;
    ctx->req_->body_.assign(at, length);
    return 0;
}

int on_message_complete(http_parser* parser)
{
    // todo
    return 0;
}

int on_chunk_header(http_parser* parser)
{
    // todo
    return 0;
}
int on_chunk_complete(http_parser* parser)
{
    // todo
    return 0;
}