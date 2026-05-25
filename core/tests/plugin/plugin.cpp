#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/traits/field.hpp"

#include <gtest/gtest.h>

using namespace rome;
using namespace rome::core;

struct HostPluginState {
    i32 value = 0;

    RM_REFLECT;
};
RM_REFLECT_IMPL(HostPluginState, "HostPluginState", Fields().with("value", &HostPluginState::value));

TEST(Plugin, Load_ValidDylib_RegistersSystem) {
    ECS ecs;

    Plugin::ID plugin = ecs.loadPlugin(RM_TEST_PLUGIN_PATH);

    EXPECT_NE(plugin, Plugin::INVALID_ID);
    EXPECT_EQ(ecs.getPluginCount(), 1);
    EXPECT_EQ(ecs.registerSystem(ecs.createSystem("test.plugin").build([](System::Context&) {})), System::INVALID_ID);
}

TEST(Plugin, Unload_ValidDylib_RemovesSystem) {
    ECS ecs;
    Plugin::ID plugin = ecs.loadPlugin(RM_TEST_PLUGIN_PATH);

    ASSERT_NE(plugin, Plugin::INVALID_ID);

    EXPECT_TRUE(ecs.unloadPlugin(plugin));
    EXPECT_EQ(ecs.getPluginCount(), 0);
    EXPECT_NE(ecs.registerSystem(ecs.createSystem("test.plugin").writes<HostPluginState>().requireFull().build([](System::Context&) {})),
              System::INVALID_ID);
}

TEST(Plugin, Load_MissingDylib_Fails) {
    ECS ecs;

    EXPECT_EQ(ecs.loadPlugin("/missing/plugin.dylib"), Plugin::INVALID_ID);
    EXPECT_EQ(ecs.getPluginCount(), 0);
}

TEST(Plugin, Load_PluginDependency_LoadsRelativeDependency) {
    ECS ecs;

    Plugin::ID plugin = ecs.loadPlugin(RM_TEST_DEPENDENT_PLUGIN_PATH);

    EXPECT_NE(plugin, Plugin::INVALID_ID);
    EXPECT_EQ(ecs.getPluginCount(), 2);
    EXPECT_EQ(ecs.registerSystem(ecs.createSystem("test.plugin.dependency").build([](System::Context&) {})), System::INVALID_ID);
    EXPECT_EQ(ecs.registerSystem(ecs.createSystem("test.plugin.dependent").build([](System::Context&) {})), System::INVALID_ID);
}

TEST(Plugin, Unload_PluginDependency_RemovesDependency) {
    ECS ecs;
    Plugin::ID plugin = ecs.loadPlugin(RM_TEST_DEPENDENT_PLUGIN_PATH);

    ASSERT_NE(plugin, Plugin::INVALID_ID);

    EXPECT_TRUE(ecs.unloadPlugin(plugin));
    EXPECT_EQ(ecs.getPluginCount(), 0);
    ecs.registerComponent<HostPluginState>();
    EXPECT_NE(ecs.registerSystem(ecs.createSystem("test.plugin.dependency").writes<HostPluginState>().requireFull().build([](System::Context&) {})),
              System::INVALID_ID);
    EXPECT_NE(ecs.registerSystem(ecs.createSystem("test.plugin.dependent").writes<HostPluginState>().requireFull().build([](System::Context&) {})),
              System::INVALID_ID);
}
