#pragma once

#include <shared_mutex>

#include "container/bitset.hpp"
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
             * @brief Registers a component type.
             * @tparam T The component type to register.
             * @return The ID of the registered component.
             * @note This function is thread-safe.
             */
            template <Component T>
            ID enter() {
                static const std::string_view name = Reflect::reflect<T>().getType().getName();

                {
                    std::shared_lock read(idsLock);
                    auto it = ids.find(name);
                    if (it != ids.end()) return it->second;
                }

                std::unique_lock write(idsLock);
                auto [it, inserted] = ids.emplace(name, nextId.load());
                if (inserted) {
                    ID id = nextId.fetch_add(1);
                    it->second = id;
                    names.emplace(id, std::string(name));
                    store.emplace(id, std::make_unique<Pool<T>>());
                    return id;
                }
                return it->second;
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam T The component type to create.
             * @param entity The entity to create the component for.
             * @return The created component or None.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            [[nodiscard]] OptRef<T> create(const Entity& entity, T& component) {
                Pool<T>* pool = getPool<T>();
                pool->insert(entity, component);
                return pool->get(entity);
            }

            /**
             * @brief Creates a new component in-place for the given entity.
             * @tparam T The component type to create.
             * @tparam ...Args The types of the arguments to forward to the component constructor.
             * @param entity The entity to create the component for.
             * @param ...args The arguments to forward to the component constructor.
             * @return The created component or none.
             * @warning This function is not thread-safe.
             */
            template <Component T, typename... Args>
            [[nodiscard]] OptRef<T> emplace(const Entity& entity, Args&&... args) {
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
             * @return The component for the given entity or None.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            [[nodiscard]] OptRef<T> get(const Entity& entity) noexcept {
                return getPool<T>()->get(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity or None.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            [[nodiscard]] OptRef<const T> get(const Entity& entity) const noexcept {
                return getPool<T>()->get(entity);
            }

            /**
             * @brief Calculates the total number of component types in the registry.
             * @return The total number of component types.
             * @warning This function is not thread-safe.
             */
            u32 getSize() const noexcept;

            /**
             * @brief Checks if an entity has the given component.
             * @tparam T The component type to check.
             * @param entity The entity to check.
             * @return True if the entity has the component, false otherwise.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            b8 has(const Entity& entity) const noexcept {
                return getPool<T>()->contains(entity);
            }

            /**
             * @brief Checks if an entity's archetype contains all the given components.
             * @param entity The entity to check.
             * @param components The set of components to check against.
             * @return True if the entity's archetype contains every component, false otherwise.
             * @warning This function is not thread-safe.
             * @note The archetype does not need to exactly match the given set of components.
             */
            b8 contains(const Entity& entity, const BitSet<>& components) noexcept;

            /**
             * @brief Gets the name of a component type given its ID.
             * @return The name of the component type.
             * @warning This function is not thread-safe.
             */
            [[nodiscard]] std::string getName(ID id) const;

            /**
             * @brief Fetches the concrete pool for the given component type.
             * @tparam T The component type to fetch the pool for.
             * @return The pool for the given component type.
             * @note This function is thread-safe.
             */
            template <Component T>
            Pool<T>* getPool() {
                ID id = getID<T>();
                std::shared_lock readLock(idsLock);
                auto it = store.find(id);
                return static_cast<Pool<T>*>(it != store.end() ? it->second.get() : nullptr);
            }

            private:
            mutable std::shared_mutex idsLock;                                            ///< Ensure thread-safe access to the IDs map.
            std::unordered_map<ID, Unique<Storage>> store;                                ///< Storage for component pools.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps component names to their IDs.
            std::unordered_map<ID, std::string> names;                                    ///< Reverse lookup.
            std::atomic_uint32_t nextId{0};                                               ///< The next available ID for a component.
            std::unordered_map<u64, BitSet<>> archetypes;                                 ///< Maps entity IDs to their archetype signatures.

            /**
             * @brief Gets the component ID for the given component type.
             * @tparam T The component type to get the ID for.
             * @return The ID of the component type.
             * @note This function is thread-safe.
             */
            template <Component T>
            ID getID() {
                return enter<T>();
            }
        };
    }  // namespace Component
}  // namespace rome::core
