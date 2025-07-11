#pragma once

#include <shared_mutex>

#include "ecs/component/pool.hpp"

namespace rome::core {
    namespace Component {
        /**
         * @brief Manages the registration, creation, and destruction of components.
         * @note This registry is not thread-safe outside component registration.
         * @warning Every component must implement reflection and a copy-constructor to be registered.
         */
        class RM_API Registry final {
            public:
            Registry() = default;
            ~Registry() = default;
            Registry(const Registry& other) = delete;
            Registry(Registry&& other) noexcept = delete;
            Registry& operator=(const Registry& other) = delete;
            Registry& operator=(Registry&& other) noexcept = delete;

            /**
             * @brief Registers a component type with the registry.
             * @tparam T The component type to register.
             * @return The ID of the registered component.
             * @note This is not technically necessary but should be used as a sanity check.
             *       This function is thread-safe.
             */
            template <Component T>
            ID enter() {
                return getID<T>();
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam T The component type to create.
             * @param entity The entity to create the component for.
             * @return The created component.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            T& create(const Entity& entity, T& component) {
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
             * @warning This function is not thread-safe.
             */
            template <Component T, typename... Args>
            T& create(const Entity& entity, Args&&... args) {
                Pool<T>* pool = getPool<T>();
                pool->emplace(entity, std::forward<Args>(args)...);
                return pool->get(entity);
            }

            /**
             * @brief Removes a component from a given entity.
             * @tparam T The component type.
             * @param entity The entity to remove the component from.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            void remove(const Entity& entity) {
                getPool<T>()->remove(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            T& get(const Entity& entity) {
                return getPool<T>()->get(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            const T& get(const Entity& entity) const {
                return getPool<T>()->get(entity);
            }

            /**
             * @brief Calculates the total number of component types in the registry.
             * @return The total number of component types.
             * @warning This function is not thread-safe.
             */
            u32 count() const;

            /**
             * @brief Gets the name of a component type given its ID.
             * @return The name of the component type.
             * @warning This function is not thread-safe.
             */
            const std::string& name(ID id) const;

            private:
            mutable std::shared_mutex idsLock;                                            ///< Ensure thread-safe access to the IDs map.
            std::unordered_map<ID, Unique<Storage>> store;                                ///< Storage for component pools.
            std::unordered_map<ID, std::string> names;                                    ///< Maps component IDs to their names.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps component names to their IDs.
            std::atomic_uint32_t nextId{0};                                               ///< The next available ID for a component.

            /**
             * @brief Gets the component ID for the given component type.
             * @tparam T The component type to get the ID for.
             * @return The ID of the component type.
             * @note This function is thread-safe.
             */
            template <Component T>
            ID getID() {
                static std::string_view name = Reflect::reflect<T>().getType().getName();
                static constinit std::atomic<ID> cached{std::numeric_limits<ID>::max()};

                ID id = cached.load(std::memory_order_acquire);
                if (id != std::numeric_limits<ID>::max()) return id;

                {
                    std::shared_lock readLock(idsLock);
                    auto it = ids.find(name);
                    if (it != ids.end()) {
                        id = it->second;
                        cached.store(id, std::memory_order_release);
                        return id;
                    }
                }

                std::unique_lock writeLock(idsLock);

                auto it = ids.find(name);
                if (it != ids.end()) {
                    id = it->second;
                } else {
                    id = nextId++;
                    ids.emplace(name, id);
                    names.emplace(id, name);
                    store.emplace(id, std::make_unique<Pool<T>>());
                }

                cached.store(id, std::memory_order_release);
                return id;
            }

            /**
             * @brief Fetches the concrete pool for the given component type.
             * @tparam T The component type to fetch the pool for.
             * @return The pool for the given component type.
             * @note This function is thread-safe.
             */
            template <Component T>
            Pool<T>* getPool() {
                static Pool<T>* cache = [this] {
                    ID id = getID<T>();
                    std::shared_lock readLock(idsLock);
                    auto it = store.find(id);
                    return static_cast<Pool<T>*>(it->second.get());
                }();
                return cache;
            }
        };
    }  // namespace Component
}  // namespace rome::core