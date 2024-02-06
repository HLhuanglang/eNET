#include "buffer.h"

#include <gtest/gtest.h>

#include <cstring>
#include <iostream>
#include <string>

TEST(BufferTest, BasicTest) {
    EasyNet::Buffer *buff = new EasyNet::Buffer();
    EXPECT_EQ(buff->GetReadableSize(), 0);
    EXPECT_EQ(buff->GetWriteableSize(), EasyNet::BufferDetail::KInitalSize);

    const std::string str(200, 'x');
    buff->Append(str);
    EXPECT_EQ(buff->GetReadableSize(), str.size());
    EXPECT_EQ(buff->GetWriteableSize(), EasyNet::BufferDetail::KInitalSize - str.size());

    const std::string str2 = buff->RetriveAsString(50);
    EXPECT_EQ(buff->GetPrependableSize(), 50);
    EXPECT_EQ(str2.size(), 50);
    EXPECT_EQ(buff->GetReadableSize(), str.size() - str2.size());
    EXPECT_EQ(buff->GetWriteableSize(), EasyNet::BufferDetail::KInitalSize - str.size());
    EXPECT_EQ(str2, std::string(50, 'x'));

    buff->Append(str);
    EXPECT_EQ(buff->GetReadableSize(), 2 * str.size() - str2.size());
    EXPECT_EQ(buff->GetWriteableSize(), EasyNet::BufferDetail::KInitalSize - 2 * str.size());

    const std::string str3 = buff->RetriveAllAsString();
    EXPECT_EQ(str3.size(), 350);
    EXPECT_EQ(buff->GetReadableSize(), 0);
    EXPECT_EQ(buff->GetWriteableSize(), EasyNet::BufferDetail::KInitalSize);
    EXPECT_EQ(str3, std::string(350, 'x'));
}

TEST(BufferTest, AppendTest) {
    EasyNet::Buffer *buff = new EasyNet::Buffer();
    buff->Append("Hello,word!");
    auto retStr1 = buff->RetriveAsString(6);
    auto retStr = buff->RetriveAllAsString();
    EXPECT_EQ(retStr1, "Hello,");
    EXPECT_EQ(retStr, "word!");
}