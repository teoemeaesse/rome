#include <gtest/gtest.h>

#include <string>

#include "rm/debug/exception.hpp"

using namespace rome::core;

TEST(Exception, Constructors_Custom_OK) {
    Exception exception(Exception::Type::InvalidArgument, "bad argument", "test.cpp", 12, "test");

    EXPECT_EQ(exception.getType(), Exception::Type::InvalidArgument);
    EXPECT_NE(std::string(exception.what()).find("InvalidArgument"), std::string::npos);
    EXPECT_NE(std::string(exception.what()).find("bad argument"), std::string::npos);
    EXPECT_NE(std::string(exception.what()).find("test.cpp:12"), std::string::npos);
}

TEST(Exception, WithFollowUp_Always_AppendsFrameAndUpdatesType) {
    Exception exception(Exception::Type::NotFound, "missing", "first.cpp", 1, "first");

    exception.withFollowUp(Exception::Type::NotSupported, "unsupported", "second.cpp", 2, "second");

    EXPECT_EQ(exception.getType(), Exception::Type::NotSupported);
    EXPECT_NE(std::string(exception.what()).find("#0 - NotFound"), std::string::npos);
    EXPECT_NE(std::string(exception.what()).find("#1 - NotSupported"), std::string::npos);
    EXPECT_NE(std::string(exception.what()).find("unsupported"), std::string::npos);
}
