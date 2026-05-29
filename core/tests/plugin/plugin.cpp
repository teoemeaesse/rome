#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/traits/field.hpp"

#include <gtest/gtest.h>

#include <utility>

using namespace rome;
using namespace rome::core;

struct HostPluginState {
    i32 value = 0;

    RM_REFLECT;
};
RM_REFLECT_IMPL(HostPluginState, "HostPluginState", Fields().with("value", &HostPluginState::value));

TEST(PluginBuilder, Build_WithPath_OK) {
    ECS ecs;

    Plugin::Descriptor descriptor = ecs.createPlugin("/tmp/test-plugin.dylib").build();

    EXPECT_EQ(descriptor.path, "/tmp/test-plugin.dylib");
}

TEST(Plugin, Submit_ValidDylib_SubmitsSystem) {
    ECS ecs;
    Plugin::Descriptor descriptor = ecs.createPlugin(RM_TEST_PLUGIN_PATH).build();

    EXPECT_TRUE(ecs.submitPlugin(std::move(descriptor)));
    Plugin::ID plugin = ecs.getPluginID(RM_TEST_PLUGIN_PATH);

    EXPECT_NE(plugin, Plugin::INVALID_ID);
    EXPECT_TRUE(ecs.checkPlugin(plugin));
    EXPECT_EQ(ecs.getPluginCount(), 1);
    EXPECT_FALSE(ecs.submitSystem(ecs.createSystem("test.plugin").build([](System::Context&) {})));
}

TEST(Plugin, Revoke_ValidDylib_RevokesSystem) {
    ECS ecs;
    Plugin::Descriptor descriptor = ecs.createPlugin(RM_TEST_PLUGIN_PATH).build();

    EXPECT_TRUE(ecs.submitPlugin(std::move(descriptor)));
    Plugin::ID plugin = ecs.getPluginID(RM_TEST_PLUGIN_PATH);

    ASSERT_NE(plugin, Plugin::INVALID_ID);

    EXPECT_TRUE(ecs.revokePlugin(plugin));
    EXPECT_FALSE(ecs.checkPlugin(plugin));
    EXPECT_EQ(ecs.getPluginCount(), 0);
    EXPECT_TRUE(ecs.submitSystem(ecs.createSystem("test.plugin").writes<HostPluginState>().requireFull().build([](System::Context&) {})));
}

TEST(Plugin, Reload_ValidDylib_AfterRevoke_OK) {
    ECS ecs;
    Plugin::Descriptor firstDescriptor = ecs.createPlugin(RM_TEST_PLUGIN_PATH).build();

    EXPECT_TRUE(ecs.submitPlugin(std::move(firstDescriptor)));
    Plugin::ID first = ecs.getPluginID(RM_TEST_PLUGIN_PATH);

    ASSERT_NE(first, Plugin::INVALID_ID);
    EXPECT_TRUE(ecs.revokePlugin(first));
    EXPECT_EQ(ecs.getPluginCount(), 0);

    Plugin::Descriptor secondDescriptor = ecs.createPlugin(RM_TEST_PLUGIN_PATH).build();

    EXPECT_TRUE(ecs.submitPlugin(std::move(secondDescriptor)));
    Plugin::ID second = ecs.getPluginID(RM_TEST_PLUGIN_PATH);

    EXPECT_NE(second, Plugin::INVALID_ID);
    EXPECT_EQ(ecs.getPluginCount(), 1);
}

TEST(Plugin, Submit_MissingDylib_Fails) {
    ECS ecs;
    Plugin::Descriptor descriptor = ecs.createPlugin("/missing/plugin.dylib").build();

    EXPECT_FALSE(ecs.submitPlugin(std::move(descriptor)));
    EXPECT_EQ(ecs.getPluginCount(), 0);
}

TEST(Plugin, Submit_PluginDependency_SubmitsRelativeDependency) {
    ECS ecs;
    Plugin::Descriptor descriptor = ecs.createPlugin(RM_TEST_DEPENDENT_PLUGIN_PATH).build();

    EXPECT_TRUE(ecs.submitPlugin(std::move(descriptor)));
    Plugin::ID plugin = ecs.getPluginID(RM_TEST_DEPENDENT_PLUGIN_PATH);

    EXPECT_NE(plugin, Plugin::INVALID_ID);
    EXPECT_EQ(ecs.getPluginCount(), 2);
    EXPECT_FALSE(ecs.submitSystem(ecs.createSystem("test.plugin.dependency").build([](System::Context&) {})));
    EXPECT_FALSE(ecs.submitSystem(ecs.createSystem("test.plugin.dependent").build([](System::Context&) {})));
}

TEST(Plugin, Revoke_PluginDependency_RevokesDependency) {
    ECS ecs;
    Plugin::Descriptor descriptor = ecs.createPlugin(RM_TEST_DEPENDENT_PLUGIN_PATH).build();

    EXPECT_TRUE(ecs.submitPlugin(std::move(descriptor)));
    Plugin::ID plugin = ecs.getPluginID(RM_TEST_DEPENDENT_PLUGIN_PATH);

    ASSERT_NE(plugin, Plugin::INVALID_ID);

    EXPECT_TRUE(ecs.revokePlugin(plugin));
    EXPECT_EQ(ecs.getPluginCount(), 0);
    ecs.submitComponent<HostPluginState>();
    EXPECT_TRUE(
        ecs.submitSystem(ecs.createSystem("test.plugin.dependency").writes<HostPluginState>().requireFull().build([](System::Context&) {})));
    EXPECT_TRUE(
        ecs.submitSystem(ecs.createSystem("test.plugin.dependent").writes<HostPluginState>().requireFull().build([](System::Context&) {})));
}
