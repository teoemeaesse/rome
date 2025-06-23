#pragma once

#include "ecs/component/pool.hpp"

namespace iodine::core {
    namespace Component {
        /**
         * @brief Manages the registration, creation, and destruction of components.
         *        Components must be registered before they can be used in the ECS.
         *        Every component must implement reflection and a copy-constructor to be registered.
         */
        class IO_API Registry {
            public:
            Registry() = default;
            ~Registry() = default;

            /**
             * @brief Registers a component type with the registry.
             * @tparam T The component type to register.
             * @return The ID of the registered component.
             */
            template <Component T>
            const UUID& registerComponent() {
                static const UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) == store.end(), "Duplicate component registration");
                store[id] = MakeUnique<Pool<T>>();
                return id;
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam T The component type to create.
             * @param entity The entity to create the component for.
             * @return The created component.
             */
            template <Component T>
            T& createComponent(const Entity& entity, T& component) {
                Pool<T>* pool = getPool<T>();
                pool->insert(entity, component);
                return pool->get(entity);
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam T The component type to create.
             * @tparam Args The types of the arguments to forward to the component constructor.
             * @param entity The entity to create the component for.
             * @param ...args The arguments to forward to the component constructor.
             * @return The created component.
             */
            template <Component T, typename... Args>
            T& createComponent(const Entity& entity, Args&&... args) {
                Pool<T>* pool = getPool<T>();
                pool->emplace(entity, std::forward<Args>(args)...);
                return pool->get(entity);
            }

            /**
             * @brief Removes a component from a given entity.
             * @tparam T The component type.
             * @param entity The entity to remove the component from.
             */
            template <Component T>
            void removeComponent(const Entity& entity) {
                getPool<T>()->remove(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            template <Component T>
            T& getComponent(const Entity& entity) {
                return getPool<T>()->get(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            template <Component T>
            const T& getComponent(const Entity& entity) const {
                return getPool<T>()->get(entity);
            }

            private:
            std::unordered_map<UUID, Unique<Storage>> store;  ///< The storage for component types.\

            /**
             * @brief Fetches the concrete pool for the given component type.
             * @tparam T The component type to fetch the pool for.
             * @return The pool for the given component type.
             */
            template <Component T>
            Pool<T>* getPool() const {
                const UUID id = Reflect::reflect<T>().getType().getUUID();
                auto it = store.find(id);
                IO_ASSERT_MSG(it != store.end(), "Component not registered");
                return static_cast<Pool<T>*>(it->second.get());
            }
        };
    }  // namespace Component
}  // namespace iodine::core