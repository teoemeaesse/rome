#include "rm/app/app.hpp"
#include "rm/debug/log.hpp"
#include "rm/debug/metrics.hpp"
#include "rm/ecs/ecs.hpp"
#include "rm/entry/entry.hpp"
#include "rm/physics/newton.hpp"

using namespace rome;
using namespace rome::core;
using namespace rome::engine::physics;

class SandboxApplication final : public Application {
    public:
    SandboxApplication()
        : Application(Application::Builder().setTitle("Rome Sandbox").setTickRate(60).setRenderRate(60).enableMemoryLogging().build()) {}

    void setup() override {
        ecs.submitPlugin(RM_NEWTON_PLUGIN_PATH);
        plugin = ecs.getPluginID(RM_NEWTON_PLUGIN_PATH);
        if (plugin == Plugin::INVALID_ID) {
            RM_ERROR("Failed to load Newton physics plugin: %s", RM_NEWTON_PLUGIN_PATH);
            stop();
            return;
        }

        body.emplace(ecs.createEntity());
        ecs.addComponent<Position>(*body, Position{0.0f, 0.0f});
        ecs.addComponent<Velocity>(*body, Velocity{1.0f, 0.0f});
        ecs.addComponent<Acceleration>(*body, Acceleration{0.0f, -9.8f});
        ecs.addComponent<NewtonIntegrator>(*body, NewtonIntegrator{1.0f / 60.0f});
    }

    void shutdown() override {
        ecs.revokePlugins();
        Metrics::getInstance().report();
    }

    void tick(f64) override {
        ecs.runSystems();
        ticks++;

        if (ticks == 60 && body.has_value()) {
            const Position& position = ecs.getComponent<Position>(*body);
            const Velocity& velocity = ecs.getComponent<Velocity>(*body);
            RM_INFO("Newton sample after 60 ticks: position=(%.2f, %.2f), velocity=(%.2f, %.2f)", position.x, position.y, velocity.x, velocity.y);
            stop();
        }
    }

    void render(f64) override {}

    private:
    ECS ecs;
    Plugin::ID plugin = Plugin::INVALID_ID;
    std::optional<Entity> body;
    u32 ticks = 0;
};

Application* createApplication() { return new SandboxApplication(); }
