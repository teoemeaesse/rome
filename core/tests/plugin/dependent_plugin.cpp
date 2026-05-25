#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

struct DependentPluginState {
    i32 value = 0;

    RM_REFLECT;
};
RM_REFLECT_IMPL(DependentPluginState, "DependentPluginState", Fields().with("value", &DependentPluginState::value));

static Plugin::ID dependencyPluginId = Plugin::INVALID_ID;
static System::ID dependentSystemId = System::INVALID_ID;

extern "C" RM_API void rome_load_plugin(ECS& ecs) {
    dependencyPluginId = ecs.loadPlugin(RM_TEST_DEPENDENCY_PLUGIN_FILE);
    ecs.registerComponent<DependentPluginState>();
    dependentSystemId =
        ecs.registerSystem(ecs.createSystem("test.plugin.dependent").writes<DependentPluginState>().requireFull().build([](System::Context&) {}));
}

extern "C" RM_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.unregisterSystem(dependentSystemId)) dependentSystemId = System::INVALID_ID;

    if (ecs.unloadPlugin(dependencyPluginId)) dependencyPluginId = Plugin::INVALID_ID;
}
