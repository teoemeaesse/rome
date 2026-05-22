#include "rm/concurrency/thread.hpp"

#include <gtest/gtest.h>

using namespace rome::core;

TEST(ThreadInfo, LocalAlias_Default_IsMain) { EXPECT_EQ(ThreadInfo::getLocalAlias(), "Main"); }

TEST(ThreadInfo, SetLocalAlias_Always_UpdatesAlias) {
    const std::string previous = ThreadInfo::getLocalAlias();

    ThreadInfo::setLocalAlias("ThreadInfoTest");
    EXPECT_EQ(ThreadInfo::getLocalAlias(), "ThreadInfoTest");

    ThreadInfo::setLocalAlias(previous);
}

TEST(Thread, Run_Always_InjectsAliasAndID) {
    Thread thread("Worker");
    std::string alias;
    UUID id;

    thread.run([&]() {
        alias = ThreadInfo::getLocalAlias();
        id = ThreadInfo::getLocalID();
    });

    EXPECT_TRUE(thread.isRunning());
    thread.join();

    EXPECT_FALSE(thread.isRunning());
    EXPECT_EQ(alias, "Worker");
    EXPECT_EQ(id, thread.getID());
    EXPECT_EQ(thread.getAlias(), "Worker");
}
