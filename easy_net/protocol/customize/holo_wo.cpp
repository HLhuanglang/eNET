#include "holo_wo.h"
#include <cstddef>

encode_state encode_holo_wo_pkg(const char* buf, size_t buf_size, holo_wo_t& holo_wo)
{
    auto head_size = sizeof(holo_wo_head_t);
    if (buf_size < head_size) {
        return encode_state::HEAD_Incomplete;
    }

    auto head             = (holo_wo_head_t*)buf;
    holo_wo.head.version  = head->version;
    holo_wo.head.length   = head->length;
    holo_wo.head.checksum = head->checksum;

    if (holo_wo.head.length > buf_size - head_size) {
        return encode_state::BODY_Incomplete;
    }
    holo_wo.body.data.assign(buf + head_size, buf_size - head_size);
    return encode_state::SUCCESS;
}