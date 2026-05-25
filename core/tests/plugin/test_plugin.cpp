#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

struct PluginState {
    i32 value = 0;

    RM_REFLECT;
};
RM_REFLECT_IMPL(PluginState, "PluginState", Fields().with("value", &PluginState::value));

static System::ID systemId = System::INVALID_ID;

extern "C" RM_API void rome_load_plugin(ECS& ecs) {
    ecs.registerComponent<PluginState>();
    systemId = ecs.registerSystem(ecs.createSystem("test.plugin").writes<PluginState>().requireFull().build([](System::Context&) {}));
}

extern "C" RM_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.unregisterSystem(systemId)) systemId = System::INVALID_ID;
}
