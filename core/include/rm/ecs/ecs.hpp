#pragma once

#include "rm/ecs/system/registry.hpp"
#include "rm/ecs/world.hpp"
#include "rm/plugin/registry.hpp"

namespace rome::core {
    /**
     * @brief The "ECS" is the primary interface for the ECS (Entity-Component-System) framework.
     */
    class ECS {
        public:
        ECS() : systems(), components(), entities(), events(), world{systems, components, entities, events} {}
        ~ECS() { revokePlugins(); }
        ECS(const ECS&) = delete;
        ECS& operator=(const ECS&) = delete;
        ECS(ECS&&) = delete;
        ECS& operator=(ECS&&) = delete;

        /**
         * @brief Submits a new component type declaration with the ECS.
         * @tparam T The component type to submit.
         * @return True if the component type was newly submitted, false otherwise.
         * @note This should be used by plugins on load.
         */
        template <Component::Component T>
        b8 submitComponent() {
            return components.submit<T>();
        }

        /**
         * @brief Revokes a component type declaration and destroys all of its component instances.
         * @tparam T The component type to revoke.
         * @return True if the component type existed and was removed.
         * @note This should be used by plugins on unload for plugin-defined component types.
         */
        template <Component::Component T>
        b8 revokeComponent() {
            return components.revoke<T>();
        }

        /**
         * @brief Checks whether a component type declaration has been submitted.
         * @tparam T The component type to check.
         * @return True if the component type has been submitted, false otherwise.
         */
        template <Component::Component T>
        b8 checkComponent() const {
            return components.check<T>();
        }

        /**
         * @brief Gets the ID for a submitted component type declaration.
         * @tparam T The component type to look up.
         * @return The component ID, or Component::INVALID_ID if it has not been submitted.
         */
        template <Component::Component T>
        Component::ID getComponentID() const {
            return components.getID<T>();
        }

        /**
         * @brief Creates a new entity.
         * @return The created entity.
         */
        Entity createEntity() { return entities.create(); }

        /**
         * @brief Destroys an entity, removing it from every group.
         * @param entity The entity to destroy.
         */
        void destroyEntity(const Entity& entity) {
            systems.removeEntity(entity);
            entities.destroy(entity);
        }

        /**
         * @brief Submits a new system declaration.
         * @param descriptor The descriptor for the system.
         * @return True if the system was newly submitted, false otherwise.
         */
        b8 submitSystem(System::Descriptor&& descriptor) { return systems.submit(std::move(descriptor)); }

        /**
         * @brief Revokes a submitted system declaration.
         * @param id The system ID to remove.
         * @return True if the system existed and was removed, false otherwise.
         */
        b8 revokeSystem(System::ID id) { return systems.revoke(id); }

        /**
         * @brief Checks whether a system declaration has been submitted.
         * @param id The system ID to check.
         * @return True if the system exists, false otherwise.
         */
        b8 checkSystem(System::ID id) const noexcept { return systems.check(id); }

        /**
         * @brief Gets a submitted system ID by name.
         * @param name The system name to look up.
         * @return The system ID, or System::INVALID_ID if it has not been submitted.
         */
        System::ID getSystemID(const std::string& name) const noexcept { return systems.getID(name); }

        /**
         * @brief Creates a system builder bound to this ECS world.
         * @param name The unique name of the system.
         * @return A builder ready to describe and build the system.
         */
        System::Builder createSystem(const std::string& name) { return System::Builder(name, world); }

        /**
         * @brief Executes one active system.
         * @param id The ID of the system to execute.
         */
        void runSystem(System::ID id) { systems.run(id); }

        /**
         * @brief Executes every active system.
         */
        void runSystems() { systems.run(); }

        /**
         * @brief Submits a plugin dynamic library declaration.
         * @param path The plugin library path.
         * @return True if the plugin was newly submitted, false otherwise.
         */
        b8 submitPlugin(const std::string& path) { return plugins.submit(path, *this); }

        /**
         * @brief Revokes a submitted plugin dynamic library declaration.
         * @param id The submitted plugin ID.
         * @return True if the plugin existed and was revoked.
         */
        b8 revokePlugin(Plugin::ID id) { return plugins.revoke(id, *this); }

        /**
         * @brief Revokes every submitted plugin dynamic library declaration.
         */
        void revokePlugins() { plugins.revokeAll(*this); }

        /**
         * @brief Checks whether a plugin declaration has been submitted.
         * @param id The plugin ID to check.
         * @return True if the plugin exists, false otherwise.
         */
        b8 checkPlugin(Plugin::ID id) const noexcept { return plugins.check(id); }

        /**
         * @brief Gets a submitted plugin ID by path.
         * @param path The plugin path to look up.
         * @return The plugin ID, or Plugin::INVALID_ID if it has not been submitted.
         */
        Plugin::ID getPluginID(const std::string& path) const { return plugins.get(path); }

        /**
         * @brief Gets the number of submitted plugins.
         * @return The number of submitted plugins.
         */
        u32 getPluginCount() const noexcept { return plugins.getSize(); }

        /**
         * @brief Adds a component to the given entity.
         * @tparam T The component type to add.
         * @param entity The entity to add the component to.
         * @return The created component.
         */
        template <Component::Component T>
        T& addComponent(const Entity& entity) {
            T* component = components.emplace<T>(entity);
            systems.updateEntity(entity);
            return *component;
        }

        /**
         * @brief Adds a component to the given entity.
         * @tparam T The component type to add.
         * @tparam Args The types of the arguments to forward to the component constructor.
         * @param entity The entity to add the component to.
         * @param ...args The arguments to forward to the component constructor.
         * @return The created component.
         */
        template <Component::Component T, typename... Args>
        T& addComponent(const Entity& entity, Args&&... args) {
            T* component = components.emplace<T>(entity, std::forward<Args>(args)...);
            systems.updateEntity(entity);
            return *component;
        }

        /**
         * @brief Removes a component from the given entity.
         * @tparam T The component type to remove.
         * @param entity The entity to remove the component from.
         */
        template <Component::Component T>
        void removeComponent(const Entity& entity) {
            components.remove<T>(entity);
            systems.updateEntity(entity);
        }

        /**
         * @brief Gets the component for the given entity.
         * @tparam T The component type to get.
         * @param entity The entity to get the component for.
         * @return The component for the given entity.
         */
        template <Component::Component T>
        T& getComponent(const Entity& entity) {
            return *components.get<T>(entity);
        }

        /**
         * @brief Gets the component for the given entity.
         * @tparam T The component type to get.
         * @param entity The entity to get the component for.
         * @return The component for the given entity.
         */
        template <Component::Component T>
        const T& getComponent(const Entity& entity) const {
            return *components.get<T>(entity);
        }

        /**
         * @brief Gets the current state of the ECS.
         * @return The current state of the ECS.
         */
        World& getWorld() { return world; }

        private:
        System::Registry systems;        ///< The registry for all systems in the ECS.
        Component::Registry components;  ///< The registry for all components in the ECS.
        Entity::Registry entities;       ///< The registry for all entities in the ECS.
        Event::Registry events;          ///< The registry for all events in the ECS.
        World world;                     ///< A reference to the ECS state.
        Plugin::Registry plugins;        ///< Loaded plugin dynamic libraries.
    };
}  // namespace rome::core
