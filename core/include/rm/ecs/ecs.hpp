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
        ~ECS() { unloadPlugins(); }
        ECS(const ECS&) = delete;
        ECS& operator=(const ECS&) = delete;
        ECS(ECS&&) = delete;
        ECS& operator=(ECS&&) = delete;

        /**
         * @brief Registers a new component type with the ECS.
         * @tparam T The component type to register.
         * @return The ID for the registered component type.
         * @note This should be used by plugins on load.
         */
        template <Component::Component T>
        Component::ID registerComponent() {
            return components.submit<T>();
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
         * @brief Registers a new system.
         * @param descriptor The descriptor for the system.
         * @return The ID for the registered system, or System::INVALID_ID.
         */
        System::ID registerSystem(System::Descriptor&& descriptor) { return systems.enter(std::move(descriptor)); }

        /**
         * @brief Removes a registered system.
         * @param id The system ID to remove.
         * @return True if the system existed and was removed, false otherwise.
         */
        b8 unregisterSystem(System::ID id) { return systems.erase(id); }

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
         * @brief Loads a plugin dynamic library.
         * @param path The plugin library path.
         * @return The loaded plugin ID, or Plugin::INVALID_ID.
         */
        Plugin::ID loadPlugin(const std::string& path) { return plugins.load(path, *this); }

        /**
         * @brief Unloads a plugin dynamic library.
         * @param id The loaded plugin ID.
         * @return True if the plugin existed and was unloaded.
         */
        b8 unloadPlugin(Plugin::ID id) { return plugins.unload(id, *this); }

        /**
         * @brief Unloads every loaded plugin dynamic library.
         */
        void unloadPlugins() { plugins.unload(*this); }

        /**
         * @brief Gets the number of loaded plugins.
         * @return The number of loaded plugins.
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
