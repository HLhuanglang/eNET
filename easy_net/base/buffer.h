#ifndef __EASYNET_BUFFER_H
#define __EASYNET_BUFFER_H

#include <cstddef>
#include <string>
#include <vector>

#include "non_copyable.h"

namespace EasyNet {

namespace BufferDetail {
const size_t KInitalSize = 1024; // 默认buff大小
}

/// @brief A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
/// @note
/// buffer提供了多种读写方式
/// 1,使用Append/RetriveXXX读写数据,无需手动操作读写指针
/// 2,自己手动copy数据,需要移动读写指针
class Buffer : NonCopyable {
 public:
    /// @brief 创建一个buffer
    /// @param initialSize 初始化时buff的存储大小
    /// @return buffer实例
    explicit Buffer(size_t initialSize = BufferDetail::KInitalSize);

    /// @brief 清空当前buffer中的数据
    void Clear();

    /// @brief 写入数据到buffer
    /// @note 函数内部会自动移动写指针位置
    void Append(const std::string &data);

    /// @brief 写入数据到buffer
    /// @note 函数内部会自动移动写指针位置
    void Append(const char *data, size_t sz);

    /// @brief 取回buf中的数据
    /// @param sz 读取的数据长度
    /// @note 函数内部会自动移动读指针位置
    std::string RetriveAsString(size_t sz);

    /// @brief 将buf中的数据序列化成string
    /// @note 函数内部会自动移动读指针位置
    std::string RetriveAllAsString();

    /// @brief 获取预置区域的大小
    /// @return 预置区域大小
    size_t GetPrependableSize() const;

    /// @brief 获取可读区域的大小
    /// @return 可读区域大小
    size_t GetReadableSize() const;

    /// @brief 获取可写区域的大小
    /// @return 可写区域大小
    size_t GetWriteableSize() const;

    /// @brief 获取当前整个buf的大小
    /// @return buf大小
    size_t GetBufferSize() const;

    /// @brief 获取可读区起始地址
    /// @return addr 可读区起始地址
    char *GetReadableAddr();

    /// @brief 获取可写区起始地址
    /// @return addr 可区起始地址
    char *GetWriteableAddr();

    /// @brief 设置读指针位置
    /// @param idx 读指针位置
    void SetReaderAddr(int idx);

    /// @brief 设置写指针位置
    /// @param idx 写指针位置
    void SetWriterAddr(int idx);

    /// @brief 移动读指针
    /// @param step 移动距离
    void AdvanceReader(int step);

    /// @brief 移动写指针
    /// @param step 移动距离
    void AdvanceWriter(int setp);

 private:
    /// @brief 确保有足够的空间可以写入
    void ensureEnoughBytes(int needsz);

    /// @brief 获取起始地址
    char *begin();

    /// @brief 指针归位
    void reset();

 private:
    std::vector<char> m_data;
    size_t m_writeidx;
    size_t m_readidx;
};

} // namespace EasyNet

#endif