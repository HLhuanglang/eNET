#include "config.h"

#include <gtest/gtest.h>

using namespace EasyNet;

TEST(ConfigTest, BasicTest) {
    EXPECT_TRUE(InitSDKConf("build/easynet.ini"));
    EXPECT_EQ(SDK_LOG_DIR, "/tmp");
    EXPECT_EQ(SDK_LOG_NAME, "easynet.log");
    EXPECT_EQ(SDK_LOG_LEVEL, "debug");
    EXPECT_EQ(SDK_THREAD_NUM, "4");
}