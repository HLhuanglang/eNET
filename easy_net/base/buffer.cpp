
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>

#include "buffer.h"

using namespace EasyNet;

void Buffer::Clear() {
    m_data.clear();
    m_writeidx = 0;
    m_readidx = 0;
}

Buffer::Buffer(size_t initialSize) : m_readidx(0), m_writeidx(0) {
    m_data.resize(initialSize);
}

void Buffer::Append(const std::string &data) {
    Append(data.c_str(), data.size());
}

void Buffer::Append(const char *data, size_t sz) {
    ensureEnoughBytes(sz);
    std::copy(data, data + sz, GetWriteableAddr());
    AdvanceWriter(sz);
}

std::string Buffer::RetriveAsString(size_t sz) {
    std::string retStr;
    retStr.assign(GetReadableAddr(), sz);
    AdvanceReader(sz);
    return retStr;
}

std::string Buffer::RetriveAllAsString() {
    std::string retStr;
    auto contentLen = GetReadableSize();
    retStr.assign(GetReadableAddr(), contentLen);
    reset();
    return retStr;
}

size_t Buffer::GetPrependableSize() const {
    return m_readidx;
}

size_t Buffer::GetReadableSize() const {
    return m_writeidx - m_readidx;
}

size_t Buffer::GetWriteableSize() const {
    return m_data.size() - m_writeidx;
}

size_t Buffer::GetBufferSize() const {
    return m_data.size();
}

char *Buffer::GetReadableAddr() {
    return begin() + m_readidx;
}

char *Buffer::GetWriteableAddr() {
    return begin() + m_writeidx;
}

void Buffer::SetReaderAddr(int idx) {
    m_readidx = idx;
}

void Buffer::SetWriterAddr(int idx) {
    m_writeidx = idx;
}

void Buffer::AdvanceReader(int setp) {
    m_readidx += setp;
}

void Buffer::AdvanceWriter(int setp) {
    m_writeidx += setp;
}

void Buffer::ensureEnoughBytes(int needsz) {
    if (needsz > GetWriteableSize()) {
        if (GetPrependableSize() + GetWriteableSize() >= needsz) {
            // 整个剩余空间还足够存储,则将现有数据全部向前移动.
            auto contentlen = GetReadableSize();
            std::copy(GetReadableAddr(), GetWriteableAddr(), m_data.begin());
            m_readidx = 0;
            m_writeidx = contentlen;
        } else {
            // 不够空间了，则直接扩容
            m_data.resize((m_writeidx << 1) + needsz);
        }
    }
}

char *Buffer::begin() {
    return &*m_data.begin();
}

void Buffer::reset() {
    m_readidx = 0;
    m_writeidx = 0;
}