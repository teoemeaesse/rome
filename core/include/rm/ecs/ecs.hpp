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
        ECS();
        ~ECS();
        ECS(const ECS&) = delete;
        ECS& operator=(const ECS&) = delete;
        ECS(ECS&&) = delete;
        ECS& operator=(ECS&&) = delete;

        /**
         * @brief Submits a new component type declaration with the ECS.
         * @tparam T The component type to submit.
         * @return True if the component type was newly submitted, false otherwise.
         */
        template <Component::Component T>
        b8 submitComponent() {
            return components.submit<T>();
        }

        /**
         * @brief Revokes a component type declaration and destroys all of its component instances.
         * @tparam T The component type to revoke.
         * @return True if the component type existed and was removed.
         */
        template <Component::Component T>
        b8 revokeComponent() {
            return components.revoke<T>();
        }

        /**
         * @brief Revokes a component type declaration by name and destroys all of its component instances.
         * @param name The component type name to revoke.
         * @return True if the component type existed and was removed.
         */
        b8 revokeComponent(const std::string_view name);

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
         * @brief Checks whether a component type declaration has been submitted by name.
         * @param name The component type name to check.
         * @return True if the component type has been submitted, false otherwise.
         */
        b8 checkComponent(const std::string_view name) const;

        /**
         * @brief Gets the ID for a submitted component type declaration.
         * @tparam T The component type to look up.
         * @return The component ID, or Component::INVALID_ID if not found.
         */
        template <Component::Component T>
        Component::ID getComponentID() const {
            return components.getID<T>();
        }

        /**
         * @brief Gets the ID for a submitted component type declaration by name.
         * @param name The component type name to look up.
         * @return The component ID, or Component::INVALID_ID if not found.
         */
        Component::ID getComponentID(const std::string_view name) const;

        /**
         * @brief Gets the name for a submitted component type declaration by ID.
         * @param id The component ID to look up.
         * @return The component type name, or an empty string view if not found.
         */
        std::string_view getComponentName(Component::ID id) const;

        /**
         * @brief Gets the name for a component type declaration.
         * @tparam T The component type to look up.
         * @return The component type name.
         * @note The component does not need to be submitted.
         */
        template <Component::Component T>
        static consteval std::string_view getComponentName() noexcept {
            return Component::Registry::getName<T>();
        }

        /**
         * @brief Creates a new entity.
         * @return The created entity.
         */
        Entity createEntity();

        /**
         * @brief Destroys an entity, removing it from every group.
         * @param entity The entity to destroy.
         */
        void destroyEntity(Entity entity);

        /**
         * @brief Submits a new system declaration.
         * @param descriptor The descriptor for the system.
         * @return True if the system does not exist in the registry, false otherwise.
         */
        b8 submitSystem(System::Descriptor&& descriptor);

        /**
         * @brief Revokes a submitted system declaration.
         * @param id The system ID to remove.
         * @return True if the system exists in the registry, false otherwise.
         */
        b8 revokeSystem(System::ID id);

        /**
         * @brief Revokes a submitted system declaration by name.
         * @param name The system name to remove.
         * @return True if the system exists in the registry, false otherwise.
         */
        b8 revokeSystem(const std::string_view name);

        /**
         * @brief Checks whether a system declaration has been submitted.
         * @param id The system ID to check.
         * @return True if the system exists in the registry, false otherwise.
         */
        b8 checkSystem(System::ID id) const noexcept;

        /**
         * @brief Checks whether a system declaration has been submitted by name.
         * @param name The system name to check.
         * @return True if the system exists in the registry, false otherwise.
         */
        b8 checkSystem(const std::string_view name) const noexcept;

        /**
         * @brief Gets a submitted system ID by name.
         * @param name The system name to look up.
         * @return The system ID, or System::INVALID_ID if not found.
         */
        System::ID getSystemID(const std::string_view name) const noexcept;

        /**
         * @brief Creates a system builder bound to this ECS world.
         * @param name The unique name of the system.
         * @return A builder to describe the system.
         */
        System::Builder createSystem(const std::string_view name);

        /**
         * @brief Executes one active system.
         * @param id The ID of the system to execute.
         */
        void runSystem(System::ID id);

        /**
         * @brief Executes every active system.
         */
        void runSystems();

        /**
         * @brief Loads a plugin dynamic library.
         * @param descriptor The descriptor for the plugin.
         * @return True if the plugin loaded or was already loaded, false otherwise.
         */
        b8 loadPlugin(Plugin::Descriptor&& descriptor);

        /**
         * @brief Creates a plugin builder.
         * @param path The plugin library path.
         * @return A builder to describe the plugin.
         */
        Plugin::Builder createPlugin(const std::string_view path);

        /**
         * @brief Unloads a plugin dynamic library.
         * @param id The loaded plugin ID.
         * @return True if the plugin exists in the registry, false otherwise.
         */
        b8 unloadPlugin(Plugin::ID id);

        /**
         * @brief Unloads a plugin dynamic library by path.
         * @param path The plugin library path.
         * @return True if the plugin exists in the registry, false otherwise.
         */
        b8 unloadPlugin(const std::string_view path);

        /**
         * @brief Unloads every plugin dynamic library.
         */
        void unloadPlugins();

        /**
         * @brief Checks whether a plugin dynamic library has been loaded.
         * @param id The plugin ID to check.
         * @return True if the plugin exists in the registry, false otherwise.
         */
        b8 checkPlugin(Plugin::ID id) const noexcept;

        /**
         * @brief Checks whether a plugin dynamic library has been loaded by path.
         * @param path The plugin library path to check.
         * @return True if the plugin exists in the registry, false otherwise.
         */
        b8 checkPlugin(const std::string_view path) const;

        /**
         * @brief Gets a loaded plugin ID by path.
         * @param path The plugin path to look up.
         * @return The plugin ID, or Plugin::INVALID_ID if not found.
         */
        Plugin::ID getPluginID(const std::string_view path) const;

        /**
         * @brief Gets the number of loaded plugins.
         * @return The number of loaded plugins.
         */
        u32 getPluginCount() const noexcept;

        /**
         * @brief Adds a component to the given entity by type.
         * @tparam T The component type.
         * @param entity The entity to add.
         * @return The created component.
         */
        template <Component::Component T>
        T& addComponent(Entity entity) {
            T* component = components.emplace<T>(entity);
            systems.updateEntity(entity);
            return *component;
        }

        /**
         * @brief Adds a component to the given entity by type.
         * @tparam T The component type.
         * @tparam Args The types of the arguments to forward to the component constructor.
         * @param entity The entity to add the component to.
         * @param ...args The arguments to forward to the component constructor.
         * @return The created component.
         */
        template <Component::Component T, typename... Args>
        T& addComponent(Entity entity, Args&&... args) {
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
        void removeComponent(Entity entity) {
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
        T& getComponent(Entity entity) {
            return *components.get<T>(entity);
        }

        /**
         * @brief Gets the component for the given entity.
         * @tparam T The component type to get.
         * @param entity The entity to get the component for.
         * @return The component for the given entity.
         */
        template <Component::Component T>
        const T& getComponent(Entity entity) const {
            return *components.get<T>(entity);
        }

        private:
        System::Registry systems;        ///< The registry for all systems in the ECS.
        Component::Registry components;  ///< The registry for all components in the ECS.
        Entity::Registry entities;       ///< The registry for all entities in the ECS.
        Event::Registry events;          ///< The registry for all events in the ECS.
        Plugin::Registry plugins;        ///< Loaded plugin dynamic libraries.
        World world;                     ///< A reference to the ECS state.
    };
}  // namespace rome::core
