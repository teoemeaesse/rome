#include "rm/plugin/plugin.hpp"

#include "rm/ecs/ecs.hpp"
#include "rm/ecs/system/view.hpp"
#include "rm/newton.hpp"

using namespace rome;
using namespace rome::core;
using namespace rome::engine::physics;

static System::ID newtonIntegrationSystem = System::INVALID_ID;

extern "C" RM_PLUGIN_API void rome_load_plugin(ECS& ecs) {
    ecs.submitComponent<Position>();
    ecs.submitComponent<Velocity>();
    ecs.submitComponent<Acceleration>();
    ecs.submitComponent<NewtonIntegrator>();

    ecs.submitSystem(ecs.createSystem("engine.physics.newton.integrate")
                         .reads<Acceleration, NewtonIntegrator>()
                         .writes<Position, Velocity>()
                         .requireFull()
                         .build([](System::Context& ctx) {
                             System::View<Position, Velocity, const Acceleration, const NewtonIntegrator> view(ctx);

                             for (auto [position, velocity, acceleration, integrator] : view) {
                                 const f32 dt = integrator.dt;
                                 velocity.x += acceleration.x * dt;
                                 velocity.y += acceleration.y * dt;
                                 position.x += velocity.x * dt;
                                 position.y += velocity.y * dt;
                             }
                         }));
    newtonIntegrationSystem = ecs.getSystemID("engine.physics.newton.integrate");
}

extern "C" RM_PLUGIN_API void rome_unload_plugin(ECS& ecs) {
    if (ecs.revokeSystem(newtonIntegrationSystem)) newtonIntegrationSystem = System::INVALID_ID;
    ecs.revokeComponent<NewtonIntegrator>();
    ecs.revokeComponent<Acceleration>();
    ecs.revokeComponent<Velocity>();
    ecs.revokeComponent<Position>();
}
