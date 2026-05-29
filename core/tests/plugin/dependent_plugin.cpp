#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/traits/field.hpp"

#include <utility>

using namespace rome;
using namespace rome::core;

struct DependentPluginState {
    i32 value = 0;

    RM_REFLECT;
};
RM_REFLECT_IMPL(DependentPluginState, "DependentPluginState", Fields().with("value", &DependentPluginState::value));

static Plugin::ID dependencyPluginId = Plugin::INVALID_ID;
static System::ID dependentSystemId = System::INVALID_ID;

extern "C" RM_PLUGIN_API void rome_load_plugin(ECS& ecs) {
    Plugin::Descriptor dependency = ecs.createPlugin(RM_TEST_DEPENDENCY_PLUGIN_FILE).build();

    ecs.submitPlugin(std::move(dependency));
    dependencyPluginId = ecs.getPluginID(RM_TEST_DEPENDENCY_PLUGIN_FILE);
    ecs.submitComponent<DependentPluginState>();
    ecs.submitSystem(ecs.createSystem("test.plugin.dependent").writes<DependentPluginState>().requireFull().build([](System::Context&) {}));
    dependentSystemId = ecs.getSystemID("test.plugin.dependent");
}

extern "C" RM_PLUGIN_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.revokeSystem(dependentSystemId)) dependentSystemId = System::INVALID_ID;
    ecs.revokeComponent<DependentPluginState>();

    if (ecs.revokePlugin(dependencyPluginId)) dependencyPluginId = Plugin::INVALID_ID;
}
