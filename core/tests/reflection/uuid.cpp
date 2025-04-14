#include "reflection/uuid.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using namespace iodine::core;

TEST(UUIDTest, ValidConversion) {
    const std::string uuidStr = "550e8400-e29b-41d4-a716-446655440000";
    UUID uuid(uuidStr);

    std::string converted = static_cast<std::string>(uuid);
    EXPECT_EQ(converted, "550e8400-e29b-41d4-a716-446655440000");
}

TEST(UUIDTest, EqualityComparison) {
    const std::string uuidStr = "550e8400-e29b-41d4-a716-446655440000";
    UUID uuid1(uuidStr);
    UUID uuid2(uuidStr);
    EXPECT_TRUE(uuid1 == uuid2);

    const std::string differentStr = "550e8400-e29b-41d4-a716-446655440001";
    UUID uuid3(differentStr);
    EXPECT_TRUE(uuid1 != uuid3);

    UUID uuid4;
    UUID uuid5;
    EXPECT_TRUE(uuid4 != uuid5);
}

TEST(UUIDTest, InvalidFormatThrows) {
    // Test that constructing from an invalid string length causes an exception.
    EXPECT_THROW(
        {
            UUID uuid("550e8400-e29b-41d4-a716-44665544000");  // 35 characters (should be 36)
        },
        std::invalid_argument);

    // Test a string missing a required hyphen.
    EXPECT_THROW(
        {
            UUID uuid("550e8400e29b-41d4-a716-446655440000");  // missing hyphen at position 9
        },
        std::invalid_argument);

    // Test a string with non-hex characters.
    EXPECT_THROW(
        {
            UUID uuid("550e8400-e29b-41d4-a716-44665544zzzz");  // "zzzz" are not valid hex digits
        },
        std::invalid_argument);
}

TEST(UUIDTest, StringConstructorEquivalence) {
    // Verify that constructing a UUID from a C-style string or from an std::string yields the same result.
    const char* uuidCStr = "550e8400-e29b-41d4-a716-446655440000";
    UUID uuidFromCStr(uuidCStr);

    std::string uuidStdStr(uuidCStr);
    UUID uuidFromStdStr(uuidStdStr);

    EXPECT_TRUE(uuidFromCStr == uuidFromStdStr);
}

TEST(UUIDTest, GetVersion) {
    // For this UUID, the 7th byte is "41" (0x41) so the version is 4.
    const std::string uuidStr = "550e8400-e29b-41d4-a716-446655440000";
    UUID uuid(uuidStr);
    EXPECT_EQ(uuid.getVersion(), 4);

    UUID uuid2;
    EXPECT_EQ(uuid2.getVersion(), 1);  // Assuming the default constructor creates a UUIDv1
}