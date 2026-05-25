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

extern "C" RM_API void rome_load_plugin(ECS& ecs) {
    ecs.registerComponent<DependencyPluginState>();
    dependencySystemId =
        ecs.registerSystem(ecs.createSystem("test.plugin.dependency").writes<DependencyPluginState>().requireFull().build([](System::Context&) {}));
}

extern "C" RM_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.unregisterSystem(dependencySystemId)) dependencySystemId = System::INVALID_ID;
}
