#include "rm/plugin/library.hpp"

#include <gtest/gtest.h>

#include <utility>

#include "rm/ecs/ecs.hpp"

using namespace rome;
using namespace rome::core;

namespace {
    i32 unloadCalls = 0;

    void countUnload(ECS&) { unloadCalls++; }
}  // namespace

TEST(PluginLibrary, Construct_Always_StoresMetadata) {
    Plugin::Library library(12, "/tmp/test-plugin.dylib", nullptr, nullptr);

    EXPECT_EQ(library.getID(), 12);
    EXPECT_EQ(library.getLoadingPath(), "/tmp/test-plugin.dylib");
    EXPECT_EQ(library.getReferences(), 1);
}

TEST(PluginLibrary, UnloadFrom_WithUnloadFunction_OK) {
    ECS ecs;
    unloadCalls = 0;
    Plugin::Library library(1, "/tmp/test-plugin.dylib", nullptr, countUnload);

    library.unloadFrom(ecs);

    EXPECT_EQ(unloadCalls, 1);
}

TEST(PluginLibrary, UnloadFrom_WithoutUnloadFunction_OK) {
    ECS ecs;
    Plugin::Library library(1, "/tmp/test-plugin.dylib", nullptr, nullptr);

    library.unloadFrom(ecs);

    SUCCEED();
}

TEST(PluginLibrary, MoveConstructor_TransfersMetadata) {
    Plugin::Library source(4, "/tmp/test-plugin.dylib", nullptr, countUnload);
    source.addReference();

    Plugin::Library moved(std::move(source));

    EXPECT_EQ(moved.getID(), 4);
    EXPECT_EQ(moved.getLoadingPath(), "/tmp/test-plugin.dylib");
    EXPECT_EQ(moved.getReferences(), 2);
    EXPECT_EQ(source.getID(), Plugin::INVALID_ID);
    EXPECT_EQ(source.getReferences(), 0);
}

TEST(PluginLibrary, MoveAssignment_TransfersMetadata) {
    Plugin::Library source(7, "/tmp/source-plugin.dylib", nullptr, countUnload);
    source.addReference();
    Plugin::Library target(8, "/tmp/target-plugin.dylib", nullptr, nullptr);

    target = std::move(source);

    EXPECT_EQ(target.getID(), 7);
    EXPECT_EQ(target.getLoadingPath(), "/tmp/source-plugin.dylib");
    EXPECT_EQ(target.getReferences(), 2);
    EXPECT_EQ(source.getID(), Plugin::INVALID_ID);
    EXPECT_EQ(source.getReferences(), 0);
}
