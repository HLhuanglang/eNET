#include "inet_addr.h"
#include <gtest/gtest.h>

TEST(InetAddress, BasicTest) {
    auto addr_ipv4 = EasyNet::InetAddress("127.0.0.1", 8888, false);
    EXPECT_EQ(addr_ipv4.family(), AF_INET);
    EXPECT_EQ(addr_ipv4.SerializationToIP(), "127.0.0.1");
    EXPECT_EQ(addr_ipv4.SerializationToPort(), "8888");
    EXPECT_EQ(addr_ipv4.SerializationToIpPort(), "127.0.0.1:8888");

    auto addr_ipv6 = EasyNet::InetAddress("2001:db8:85a3::8a2e:370:7334", 8888, true);
    EXPECT_EQ(addr_ipv6.family(), AF_INET6);
    EXPECT_EQ(addr_ipv6.SerializationToIP(), "2001:db8:85a3::8a2e:370:7334");
    EXPECT_EQ(addr_ipv6.SerializationToPort(), "8888");
    EXPECT_EQ(addr_ipv6.SerializationToIpPort(), "2001:db8:85a3::8a2e:370:7334:8888");
}

TEST(InetAddress, loopbackOnly) {
    auto addr1_ipv4 = EasyNet::InetAddress(1111, true, false);
    EXPECT_EQ(addr1_ipv4.SerializationToPort(), "1111");
    EXPECT_EQ(addr1_ipv4.SerializationToIP(), "127.0.0.1");

    auto addr1_ipv6 = EasyNet::InetAddress(2222, true, true);
    EXPECT_EQ(addr1_ipv6.SerializationToPort(), "2222");
    EXPECT_EQ(addr1_ipv6.SerializationToIP(), "::1");

    auto addr2_ipv4 = EasyNet::InetAddress(3333, false, false);
    EXPECT_EQ(addr2_ipv4.SerializationToPort(), "3333");
    EXPECT_EQ(addr2_ipv4.SerializationToIP(), "0.0.0.0");

    auto addr2_ipv6 = EasyNet::InetAddress(4444, false, true);
    EXPECT_EQ(addr2_ipv6.SerializationToPort(), "4444");
    EXPECT_EQ(addr2_ipv6.SerializationToIP(), "::");
}