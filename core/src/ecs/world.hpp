#pragma once

#include "ecs/component/component.hpp"

namespace iodine::core {
    /**
     * @brief A world is a container for all archetypes, components, resources and systems and serves as the primary interface for the engine's ECS.
     */
    class World {
        public:
        World() = default;
        ~World() = default;

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

        private:
        Component::Registry components;
    };
}  // namespace iodine::core