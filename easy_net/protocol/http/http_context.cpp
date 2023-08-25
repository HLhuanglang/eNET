#include "http_context.h"
#include <cstddef>
#include <cstdlib>
#include <string>

#include "http_parser.h"

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

http_parser_settings http_context::s_parser_settings = {on_message_begin, on_url, on_status, on_header_field, on_header_value,
                                                        on_headers_complete, on_body, on_message_complete, on_chunk_header, on_chunk_complete};

size_t http_context::parser_http_context(const char *data, size_t len, http_request &req) {
    http_parser_init(&m_parser, http_parser_type::HTTP_REQUEST);
    m_parser.data = this;
    m_req = &req;
    m_type = http_type_t::HTTP_REQ;
    return http_parser_execute(&m_parser, &s_parser_settings, data, len);
}

size_t http_context::parser_http_context(const char *data, size_t len, http_response &rsp) {
    http_parser_init(&m_parser, http_parser_type::HTTP_RESPONSE);
    m_parser.data = this;
    m_rsp = &rsp;
    m_type = http_type_t::HTTP_RSP;
    return http_parser_execute(&m_parser, &s_parser_settings, data, len);
}

void http_context::handle_header() {
    if (!m_header_filed.empty() && !m_header_value.empty()) {
        if (m_type == http_type_t::HTTP_REQ) {
            m_req->m_headers[m_header_filed] = m_header_value;
        } else {
            m_rsp->m_headers[m_header_filed] = m_header_value;
        }
        m_header_filed.clear();
        m_header_value.clear();
    }
}

int on_message_begin(http_parser *parser) {
    // todo
    return 0;
}

int on_url(http_parser *parser, const char *at, size_t length) {
    // 如果是get请求,url后面会带一串数据
    auto *ctx = (http_context *)parser->data;
    if (ctx->m_type == http_type_t::HTTP_REQ) {
        ctx->m_req->m_url.assign(at, length);
    }
    return 0;
}

int on_status(http_parser *parser, const char *at, size_t length) {
    auto *ctx = (http_context *)parser->data;
    if (ctx->m_type == http_type_t::HTTP_RSP) {
        ctx->m_rsp->m_status_code_msg.assign(at, length);
    }
    return 0;
}

int on_header_field(http_parser *parser, const char *at, size_t length) {
    auto *ctx = (http_context *)parser->data;
    ctx->m_header_filed.assign(at, length);
    return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
    auto *ctx = (http_context *)parser->data;
    ctx->m_header_value.assign(at, length);
    ctx->handle_header();
    return 0;
}

int on_headers_complete(http_parser *parser) {
    auto *ctx = (http_context *)parser->data;
    if (ctx->m_type == http_type_t::HTTP_REQ) {
        const char *method = "";
#define XXX(num, name, str) \
    case num:               \
        method = #str;      \
        break;

        switch (parser->method) {
            HTTP_METHOD_MAP(XXX)
        }
#undef XXX
        ctx->m_req->m_method = method;
        ctx->m_req->m_version = "HTTP/" + std::to_string(parser->http_major) + "." + std::to_string(parser->http_minor);
    } else {
        ctx->m_rsp->m_status_code = std::to_string(parser->status_code);
        ctx->m_rsp->m_version = "HTTP/" + std::to_string(parser->http_major) + "." + std::to_string(parser->http_minor);
    }
    return 0;
}

int on_body(http_parser *parser, const char *at, size_t length) {
    auto *ctx = (http_context *)parser->data;
    if (ctx->m_type == http_type_t::HTTP_REQ) {
        auto content_length = std::atoi(ctx->m_req->m_headers["Content-Length"].c_str());
        if (content_length == static_cast<int>(length)) {
            ctx->m_req->m_body.assign(at, length);
        }
    } else {
        auto content_length = std::atoi(ctx->m_rsp->m_headers["Content-Length"].c_str());
        if (content_length == static_cast<int>(length)) {
            ctx->m_rsp->m_body.assign(at, length);
        }
    }
    return 0;
}

int on_message_complete(http_parser *parser) {
    auto *ctx = (http_context *)parser->data;
    ctx->m_finish_one_parse = true;
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