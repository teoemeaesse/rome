#include "rm/ecs/ecs.hpp"

namespace rome::core {
    ECS::ECS() : systems(), components(), entities(), events(), world{systems, components, entities, events} {}

    ECS::~ECS() {
        unloadPlugins();
    }

    b8 ECS::revokeComponent(const std::string_view name) {
        return components.revoke(name);
    }

    b8 ECS::checkComponent(const std::string_view name) const {
        return components.check(name);
    }

    Component::ID ECS::getComponentID(const std::string_view name) const {
        return components.getID(name);
    }

    std::string_view ECS::getComponentName(Component::ID id) const {
        return components.getName(id);
    }

    Entity ECS::createEntity() {
        return entities.create();
    }

    void ECS::destroyEntity(Entity entity) {
        systems.removeEntity(entity);
        entities.destroy(entity);
    }

    b8 ECS::submitSystem(System::Descriptor&& descriptor) {
        return systems.submit(std::move(descriptor));
    }

    b8 ECS::revokeSystem(System::ID id) {
        return systems.revoke(id);
    }

    b8 ECS::revokeSystem(const std::string_view name) {
        return systems.revoke(name);
    }

    b8 ECS::checkSystem(System::ID id) const noexcept {
        return systems.check(id);
    }

    b8 ECS::checkSystem(const std::string_view name) const noexcept {
        return systems.check(name);
    }

    System::ID ECS::getSystemID(const std::string_view name) const noexcept {
        return systems.getID(name);
    }

    System::Builder ECS::createSystem(const std::string_view name) {
        return System::Builder(name, world);
    }

    void ECS::runSystem(System::ID id) {
        systems.run(id);
    }

    void ECS::runSystems() {
        systems.run();
    }

    b8 ECS::loadPlugin(Plugin::Descriptor&& descriptor) {
        return plugins.load(std::move(descriptor), *this);
    }

    Plugin::Builder ECS::createPlugin(const std::string_view path) {
        return Plugin::Builder(path);
    }

    b8 ECS::unloadPlugin(Plugin::ID id) {
        return plugins.unload(id, *this);
    }

    b8 ECS::unloadPlugin(const std::string_view path) {
        return plugins.unload(path, *this);
    }

    void ECS::unloadPlugins() {
        plugins.unloadAll(*this);
    }

    b8 ECS::checkPlugin(Plugin::ID id) const noexcept {
        return plugins.check(id);
    }

    b8 ECS::checkPlugin(const std::string_view path) const {
        return plugins.check(path);
    }

    Plugin::ID ECS::getPluginID(const std::string_view path) const {
        return plugins.get(path);
    }

    u32 ECS::getPluginCount() const noexcept {
        return plugins.getSize();
    }
}  // namespace rome::core
