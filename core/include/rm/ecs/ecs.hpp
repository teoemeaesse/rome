#pragma once

#include "rm/ecs/system/registry.hpp"
#include "rm/ecs/world.hpp"

namespace rome::core {
    /**
     * @brief The "ECS" is the primary interface for the ECS (Entity-Component-System) framework.
     */
    class RM_API ECS {
        public:
        ECS() : systems(), components(), entities(), events(), world{systems, components, entities, events} {}
        ~ECS() = default;
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
    };
}  // namespace rome::core
