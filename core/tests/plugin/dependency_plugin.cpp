#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

struct DependencyPluginState {
    i32 value = 0;

    RM_REFLECT;
};
RM_REFLECT_IMPL(DependencyPluginState, "DependencyPluginState", Fields().with("value", &DependencyPluginState::value));

static System::ID dependencySystemId = System::INVALID_ID;

extern "C" RM_PLUGIN_API void rome_load_plugin(ECS& ecs) {
    ecs.submitComponent<DependencyPluginState>();
    ecs.submitSystem(ecs.createSystem("test.plugin.dependency").writes<DependencyPluginState>().requireFull().build([](System::Context&) {}));
    dependencySystemId = ecs.getSystemID("test.plugin.dependency");
}

extern "C" RM_PLUGIN_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.revokeSystem(dependencySystemId)) dependencySystemId = System::INVALID_ID;
    ecs.revokeComponent<DependencyPluginState>();
}
