#include "rm/app/app.hpp"
#include "rm/debug/log.hpp"
#include "rm/debug/metrics.hpp"
#include "rm/ecs/ecs.hpp"
#include "rm/entry/entry.hpp"
#include "rm/newton.hpp"

#include <string_view>
#include <vector>

#ifndef RM_ENGINE_PLUGIN_PATHS
#define RM_ENGINE_PLUGIN_PATHS ""
#endif

using namespace rome;
using namespace rome::core;
using namespace rome::engine::physics;

namespace {
    std::vector<std::string_view> getPluginPaths() {
        constexpr std::string_view paths = RM_ENGINE_PLUGIN_PATHS;
        std::vector<std::string_view> result;

        std::size_t start = 0;
        while (start < paths.size()) {
            const std::size_t end = paths.find('|', start);
            const std::string_view path = paths.substr(start, end == std::string_view::npos ? paths.size() - start : end - start);
            if (!path.empty()) result.push_back(path);
            if (end == std::string_view::npos) break;
            start = end + 1;
        }

        return result;
    }
}

class SandboxApplication final : public Application {
    public:
    SandboxApplication()
        : Application(Application::Builder().setTitle("Rome Sandbox").setTickRate(60).setRenderRate(60).enableMemoryLogging().build()) {}

    void setup() override {
        for (const std::string_view path : getPluginPaths()) {
            ecs.submitPlugin(ecs.createPlugin(path).build());
            const Plugin::ID plugin = ecs.getPluginID(path);
            if (plugin == Plugin::INVALID_ID) {
                RM_ERROR("Failed to load engine plugin: %.*s", static_cast<int>(path.size()), path.data());
                stop();
                return;
            }
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
    std::optional<Entity> body;
    u32 ticks = 0;
};

Application* createApplication() { return new SandboxApplication(); }
