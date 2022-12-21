#ifndef __HOLO_WO_H
#define __HOLO_WO_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#pragma pack(1)
typedef struct holo_wo_head_tag {
    uint16_t version; //协议版本
    uint32_t length;  //数据长度
    uint8_t checksum; //数据体的md5校验
} holo_wo_head_t;

typedef struct holo_wo_body_tag {
    std::string data;
} holo_wo_body_t;

typedef struct holo_wo_tag {
    holo_wo_head_t head;
    holo_wo_body_t body;
} holo_wo_t;
#pragma pack()

enum class encode_state
{
    SUCCESS = 0,
    HEAD_Incomplete,
    BODY_Incomplete,
};

encode_state encode_holo_wo_pkg(const char* buf, size_t buf_size, holo_wo_t& holo_wo);

#endif