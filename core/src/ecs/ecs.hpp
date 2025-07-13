#pragma once

#include "ecs/system/registry.hpp"

namespace rome::core {
    /**
     * @brief The "ECS" is the primary interface for the ECS (Entity-Component-System) framework.
     */
    class RM_API ECS {
        public:
        ECS() = default;
        ~ECS() = default;
        ECS(const ECS&) = delete;
        ECS& operator=(const ECS&) = delete;
        ECS(ECS&&) = default;
        ECS& operator=(ECS&&) = default;

        /**
         * @brief Registers a new component type with the ECS.
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