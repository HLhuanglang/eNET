#include "http_context.h"

const http_request &http_context::parser_http_context(const char *begin, const char *end)
{
    // todo
    bool has_more = true;
    const char *curr_idx = begin;
    while (has_more) {
        switch (curr_state_) {
        case parser_state::REQUEST_LINE:
        {
            curr_state_ = parser_state::HEADERS;
            break;
        }
        case parser_state::HEADERS:
        {
            curr_state_ = parser_state::BODY;
            break;
        }
        case parser_state::BODY:
        {
            curr_state_ = parser_state::END;
            break;
        }
        case parser_state::END:
        {
            break;
        }
        default:
            break;
        }
    }
    return *req_;
}