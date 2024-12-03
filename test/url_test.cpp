#include <gtest/gtest.h>

#include "http_url.h"

TEST(HttpTest, BasicTest) {
    EasyNet::HttpUrlParser parser("https://www.hlllz.cn/v1/api/Hi?key1=val1&key2=val2#fragment");
    EXPECT_TRUE(parser.Parse());
    EXPECT_EQ(parser.url.protocol, "https");
    EXPECT_EQ(parser.url.host, "www.hlllz.cn");
    EXPECT_EQ(parser.url.port, "443");
    EXPECT_EQ(parser.url.path, "/v1/api/Hi");
    int i = 0;
    for (auto &query : parser.url.query) {
        switch (i) {
            case 0:
                EXPECT_EQ(query.first, "key1");
                EXPECT_EQ(query.second, "val1");
            case 1:
                EXPECT_EQ(query.first, "key2");
                EXPECT_EQ(query.second, "val2");
        }
        i++;
    }
    EXPECT_EQ(parser.url.fragment, "fragment");
}

TEST(HttpTest, OnlyHasHost) {
    EasyNet::HttpUrlParser parser("http://www.hlllz.cn");
    EXPECT_TRUE(parser.Parse());
    EXPECT_EQ(parser.url.protocol, "http");
    EXPECT_EQ(parser.url.host, "www.hlllz.cn");
    EXPECT_EQ(parser.url.port, "80");
}

TEST(HttpTest, LocalHost) {
    EasyNet::HttpUrlParser parser("http://127.0.0.1");
    EXPECT_TRUE(parser.Parse());
    EXPECT_EQ(parser.url.protocol, "http");
    EXPECT_EQ(parser.url.host, "127.0.0.1");
    EXPECT_EQ(parser.url.port, "80");
}

TEST(HttpTest, LocalHostWithPort) {
    EasyNet::HttpUrlParser parser("http://127.0.0.1:8888");
    EXPECT_TRUE(parser.Parse());
    EXPECT_EQ(parser.url.protocol, "http");
    EXPECT_EQ(parser.url.host, "127.0.0.1");
    EXPECT_EQ(parser.url.port, "8888");
}