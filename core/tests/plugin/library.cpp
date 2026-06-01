#include "rm/plugin/library.hpp"

#include <gtest/gtest.h>

#include <utility>

#include "rm/ecs/ecs.hpp"

using namespace rome;
using namespace rome::core;

TEST(PluginLibrary, Construct_Always_StoresMetadata) {
    Plugin::Library library(12, Plugin::Descriptor{"/tmp/test-plugin.dylib"}, nullptr);

    EXPECT_EQ(library.getID(), 12);
    EXPECT_EQ(library.getLoadingPath(), "/tmp/test-plugin.dylib");
}

TEST(PluginLibrary, Unload_WithoutHandle_OK) {
    ECS ecs;
    Plugin::Library library(1, Plugin::Descriptor{"/tmp/test-plugin.dylib"}, nullptr);

    library.unload(ecs);

    SUCCEED();
}

TEST(PluginLibrary, MoveConstructor_TransfersMetadata) {
    Plugin::Library source(4, Plugin::Descriptor{"/tmp/test-plugin.dylib"}, nullptr);

    Plugin::Library moved(std::move(source));

    EXPECT_EQ(moved.getID(), 4);
    EXPECT_EQ(moved.getLoadingPath(), "/tmp/test-plugin.dylib");
    EXPECT_EQ(source.getID(), Plugin::INVALID_ID);
}

TEST(PluginLibrary, MoveAssignment_TransfersMetadata) {
    Plugin::Library source(7, Plugin::Descriptor{"/tmp/source-plugin.dylib"}, nullptr);
    Plugin::Library target(8, Plugin::Descriptor{"/tmp/target-plugin.dylib"}, nullptr);

    target = std::move(source);

    EXPECT_EQ(target.getID(), 7);
    EXPECT_EQ(target.getLoadingPath(), "/tmp/source-plugin.dylib");
    EXPECT_EQ(source.getID(), Plugin::INVALID_ID);
}
