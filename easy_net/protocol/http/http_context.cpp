#include "http_context.h"

#include "http_parser.h"
#include "http_response.h"
#include "log.h"

using namespace EasyNet;

static int on_url(http_parser *parser, const char *at, size_t length);
static int on_status(http_parser *parser, const char *at, size_t length);
static int on_header_field(http_parser *parser, const char *at, size_t length);
static int on_header_value(http_parser *parser, const char *at, size_t length);
static int on_body(http_parser *parser, const char *at, size_t length);
static int on_message_begin(http_parser *parser);
static int on_headers_complete(http_parser *parser);
static int on_message_complete(http_parser *parser);
static int on_chunk_header(http_parser *parser);
static int on_chunk_complete(http_parser *parser);

http_parser_settings HttpContext::s_parser_settings = {
    on_message_begin,
    on_url,
    on_status,
    on_header_field,
    on_header_value,
    on_headers_complete,
    on_body,
    on_message_complete,
    on_chunk_header,
    on_chunk_complete};

int on_message_begin(http_parser *parser) {
    // todo
    return 0;
}

int on_url(http_parser *parser, const char *at, size_t length) {
    HttpContext *ctx = (HttpContext *)parser->data;
    if (ctx->m_type == http_parser_type::HTTP_REQUEST) {
        ctx->m_req->m_url.assign(at, length);
    }
    return 0;
}

int on_status(http_parser *parser, const char *at, size_t length) {
    HttpContext *ctx = (HttpContext *)parser->data;
    if (ctx->m_type == http_parser_type::HTTP_RESPONSE) {
        ctx->m_rsp->m_status_code.assign(at, length);
    }
    return 0;
}

int on_header_field(http_parser *parser, const char *at, size_t length) {
    HttpContext *ctx = (HttpContext *)parser->data;
    ctx->m_header_filed.assign(at, length);
    return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
    HttpContext *ctx = (HttpContext *)parser->data;
    ctx->m_header_value.assign(at, length);
    if (ctx->m_header_filed.size() != 0 && ctx->m_header_value.size() != 0) {
        switch (ctx->m_type) {
            case http_parser_type::HTTP_REQUEST:
                ctx->m_req->m_headers.SetHeader(ctx->m_header_filed, ctx->m_header_value);
                break;
            case http_parser_type::HTTP_RESPONSE:
                ctx->m_rsp->m_headers.SetHeader(ctx->m_header_filed, ctx->m_header_value);
                break;
        }
        ctx->m_header_filed.clear();
        ctx->m_header_value.clear();
    }
    return 0;
}

int on_headers_complete(http_parser *parser) {
    // todo
    return 0;
}

int on_body(http_parser *parser, const char *at, size_t length) {
    HttpContext *ctx = (HttpContext *)parser->data;
    switch (ctx->m_type) {
        case http_parser_type::HTTP_REQUEST:
            ctx->m_req->m_body.assign(at, length);
            break;
        case http_parser_type::HTTP_RESPONSE:
            ctx->m_rsp->m_body.assign(at, length);
            break;
    }
    return 0;
}

int on_message_complete(http_parser *parser) {
    // todo
    return 0;
}

int on_chunk_header(http_parser *parser) {
    // todo
    return 0;
}
int on_chunk_complete(http_parser *parser) {
    // todo
    return 0;
}

HttpContext::HttpContext(http_parser_type type) : m_type(type) {
    http_parser_init(&m_parser, type);
    m_parser.data = this;
}

HttpContext::Status HttpContext::Parse(const char *data, size_t len, HttpRequest &req) {
    m_req = &req;
    auto parsed = http_parser_execute(&m_parser, &s_parser_settings, data, len);
    if (m_parser.http_errno == HPE_OK) {
        m_req->m_method = http_method_str((http_method)m_parser.method);
        m_req->m_version = "HTTP/" + std::to_string(m_parser.http_major) + "." + std::to_string(m_parser.http_minor);
        return {Status::PARSE_OK, parsed};
    } else {
        LOG_ERROR("http parse error={}", http_errno_description(HTTP_PARSER_ERRNO(&m_parser)));
        return {Status::PARSE_ERROR, parsed};
    }
}

HttpContext::Status HttpContext::Parse(const char *data, size_t len, HttpResponse &rsp) {
    m_rsp = &rsp;
    auto parsed = http_parser_execute(&m_parser, &s_parser_settings, data, len);
    if (m_parser.http_errno == HPE_OK) {
        return {Status::PARSE_OK, parsed};
    } else {
        LOG_ERROR("http parse error={}", http_errno_description(HTTP_PARSER_ERRNO(&m_parser)));
        return {Status::PARSE_ERROR, parsed};
    }
}