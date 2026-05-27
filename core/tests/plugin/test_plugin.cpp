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

extern "C" RM_PLUGIN_API void rome_load_plugin(ECS& ecs) {
    ecs.submitComponent<PluginState>();
    ecs.submitSystem(ecs.createSystem("test.plugin").writes<PluginState>().requireFull().build([](System::Context&) {}));
    systemId = ecs.getSystemID("test.plugin");
}

extern "C" RM_PLUGIN_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.revokeSystem(systemId)) systemId = System::INVALID_ID;
    ecs.revokeComponent<PluginState>();
}
