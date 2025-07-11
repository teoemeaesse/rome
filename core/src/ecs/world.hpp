#pragma once

#include "ecs/system/registry.hpp"

namespace rome::core {
    /**
     * @brief The "world" is the primary interface for the ECS (Entity-Component-System) framework.
     */
    class World {
        public:
        World() = default;
        ~World() = default;
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) = default;
        World& operator=(World&&) = default;

        struct State {
            System::Registry systems;        ///< The registry for all systems in the world.
            Component::Registry components;  ///< The registry for all components in the world.
            Entity::Registry entities;       ///< The registry for all entities in the world.
        };

        /**
         * @brief Registers a new component type with the world.
         * @tparam T The component type to register.
         * @return The ID for the registered component type.
         * @note This should be used by plugins on load.
         */
        template <typename T>
        Component::ID registerComponent() {
            return components.registerComponent<T>();
        }

        /**
         * @brief Adds a component to the given entity.
         * @tparam T The component type to add.
         * @param entity The entity to add the component to.
         * @return The created component.
         */
        template <typename T>
        T& addComponent(const Entity& entity) {
            return components.createComponent<T>(entity);
        }

        /**
         * @brief Adds a component to the given entity.
         * @tparam T The component type to add.
         * @tparam Args The types of the arguments to forward to the component constructor.
         * @param entity The entity to add the component to.
         * @param ...args The arguments to forward to the component constructor.
         * @return The created component.
         */
        template <typename T, typename... Args>
        T& addComponent(const Entity& entity, Args&&... args) {
            return components.createComponent<T>(entity, std::forward<Args>(args)...);
        }

        /**
         * @brief Removes a component from the given entity.
         * @tparam T The component type to remove.
         * @param entity The entity to remove the component from.
         */
        template <typename T>
        void removeComponent(const Entity& entity) {
            components.removeComponent<T>(entity);
        }

        /**
         * @brief Gets the component for the given entity.
         * @tparam T The component type to get.
         * @param entity The entity to get the component for.
         * @return The component for the given entity.
         */
        template <typename T>
        T& getComponent(const Entity& entity) {
            return components.getComponent<T>(entity);
        }

        /**
         * @brief Gets the component for the given entity.
         * @tparam T The component type to get.
         * @param entity The entity to get the component for.
         * @return The component for the given entity.
         */
        template <typename T>
        const T& getComponent(const Entity& entity) const {
            return components.getComponent<T>(entity);
        }

        /**
         * @brief Gets the current state of the world.
         * @return The current state of the world.
         */
        const State& getState() const { return state; }
        /**
         * @brief Gets the current state of the world.
         * @return The current state of the world.
         */
        State& getState() { return state; }

        private:
        State state;  ///< The state of the world, containing all ECS data.
    };
}  // namespace rome::core