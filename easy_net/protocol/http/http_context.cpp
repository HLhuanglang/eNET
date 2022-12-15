#include "http_context.h"
#include "http_parser.h"
#include <cstddef>
#include <string>

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

size_t http_context::parser_http_context(const char* data, size_t len, http_request& req)
{
    http_parser_init(&parser_, http_parser_type::HTTP_REQUEST);
    parser_.data = this;
    req_         = &req;
    type_        = http_type_t::HTTP_REQ;
    return http_parser_execute(&parser_, &parser_settings_, data, len);
}

size_t http_context::parser_http_context(const char* data, size_t len, http_response& rsp)
{
    http_parser_init(&parser_, http_parser_type::HTTP_RESPONSE);
    parser_.data = this;
    rsp_         = &rsp;
    type_        = http_type_t::HTTP_RSP;
    return http_parser_execute(&parser_, &parser_settings_, data, len);
}

void http_context::_handle_header()
{
    if (header_filed_.size() != 0 && header_value_.size() != 0) {
        if (type_ == http_type_t::HTTP_REQ) {
            req_->headers_[header_filed_] = header_value_;
        } else {
            rsp_->headers_[header_filed_] = header_value_;
        }
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
    if (ctx->type_ == http_type_t::HTTP_REQ) {
        ctx->req_->url_.assign(at, length);
    }
    return 0;
}

int on_status(http_parser* parser, const char* at, size_t length)
{
    http_context* ctx = (http_context*)parser->data;
    if (ctx->type_ == http_type_t::HTTP_RSP) {
        ctx->rsp_->status_code_msg_.assign(at, length);
    }
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
    http_context* ctx = (http_context*)parser->data;
    if (ctx->type_ == http_type_t::HTTP_REQ) {
        const char* method = "";
#define XXX(num, name, str) \
    case num:               \
        method = #str;      \
        break;

        switch (parser->method) {
            HTTP_METHOD_MAP(XXX)
        }
#undef XXX
        ctx->req_->method_  = method;
        ctx->req_->version_ = "HTTP/" + std::to_string(parser->http_major) + "." + std::to_string(parser->http_minor);
    } else {
        ctx->rsp_->status_code_ = std::to_string(parser->status_code);
        ctx->rsp_->version_     = "HTTP/" + std::to_string(parser->http_major) + "." + std::to_string(parser->http_minor);
    }
    return 0;
}

int on_body(http_parser* parser, const char* at, size_t length)
{
    http_context* ctx = (http_context*)parser->data;
    if (ctx->type_ == http_type_t::HTTP_REQ) {
        ctx->req_->body_.assign(at, length);
    } else {
        ctx->rsp_->body_.assign(at, length);
    }
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