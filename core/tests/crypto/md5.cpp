#include "crypto/md5.hpp"

#include "gtest/gtest.h"

using namespace iodine;
using namespace iodine::core;

struct MD5TestCase {
    std::string input;
    std::string expected;
};

TEST(MD5Test, KnownVectors) {
    // These test vectors are taken from RFC 1321.
    MD5TestCase cases[] = {{"", "d41d8cd98f00b204e9800998ecf8427e"},
                           {"a", "0cc175b9c0f1b6a831c399e269772661"},
                           {"abc", "900150983cd24fb0d6963f7d28e17f72"},
                           {"message digest", "f96b697d7cb7938d525a2f31aaf161d0"},
                           {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b"},
                           {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "d174ab98d277d9f5a5611c2c9f419d9f"},
                           {"12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a"}};

    for (const auto& test : cases) {
        MD5 md5_from_string(test.input);
        std::string result = static_cast<std::string>(md5_from_string);
        EXPECT_EQ(result, test.expected);
    }
}

TEST(MD5Test, ConstructorAndComparison) {
    std::string input = "The quick brown fox jumps over the lazy dog";
    MD5 md5_from_string(input);
    MD5 md5_from_chars(input.c_str(), static_cast<iodine::u32>(input.size()));

    EXPECT_TRUE(md5_from_string == md5_from_chars);

    EXPECT_EQ(static_cast<std::string>(md5_from_string), static_cast<std::string>(md5_from_chars));
}