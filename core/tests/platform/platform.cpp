#include "rm/platform/platform.hpp"

#include <gtest/gtest.h>

using namespace rome;
using namespace rome::core;

TEST(Platform, GetName_Always_ReturnsPlatformName) {
    Platform platform = Platform::getInstance();

    EXPECT_STREQ(platform.getName(), PLATFORM_NAME);
}

TEST(Platform, Time_Always_IsMonotonic) {
    Platform platform = Platform::getInstance();

    f64 first = platform.time();
    f64 second = platform.time();

    EXPECT_GE(second, first);
}

TEST(Platform, TimeNS_Always_IsMonotonic) {
    Platform platform = Platform::getInstance();

    u64 first = platform.timeNS();
    u64 second = platform.timeNS();

    EXPECT_GE(second, first);
}

TEST(Platform, Endianness_Always_IsExclusive) {
    Platform platform = Platform::getInstance();

    EXPECT_NE(platform.isLittleEndian(), platform.isBigEndian());
}

TEST(Platform, SwapEndian_Always_ReversesBytes) {
    Platform platform = Platform::getInstance();
    u32 value = 0x11223344;

    platform.swapEndian(&value, sizeof(value));

    EXPECT_EQ(value, 0x44332211);
}

TEST(Platform, MacOS_Always_LittleEndian) {
    Platform platform = Platform::getInstance();

    if (platform.isMacOS()) EXPECT_TRUE(platform.isLittleEndian());
}

TEST(Platform, Windows_Always_LittleEndian) {
    Platform platform = Platform::getInstance();

    if (platform.isWindows()) EXPECT_TRUE(platform.isLittleEndian());
}
