#pragma once

#include <shared_mutex>

#include "rm/container/bitset.hpp"
#include "rm/ecs/component/pool.hpp"

namespace rome::core {
    namespace Component {
        /**
         * @brief Manages the lifecycle of components within the ECS.
         * @note This registry is not thread-safe outside component registration.
         * @warning Every component must implement reflection and a copy-constructor to be submitted.
         */
        class Registry final {
            public:
            Registry() = default;
            ~Registry() = default;
            Registry(const Registry& other) = delete;
            Registry(Registry&& other) noexcept = delete;
            Registry& operator=(const Registry& other) = delete;
            Registry& operator=(Registry&& other) noexcept = delete;

            /**
             * @brief Submits a component to the registry.
             * @param name The name of the component.
             * @return True if the component did not exist in the registry, false otherwise.
             * @note This function is thread-safe.
             */
            b8 submit(const std::string_view name);

            /**
             * @brief Submits a component to the registry by type.
             * @tparam C The component type to submit.
             * @return True if the component did not exist in the registry, false otherwise.
             * @note This function is thread-safe.
             */
            template <Component C>
            b8 submit() {
                static const std::string_view name = Reflect::reflect<C>().getType().getName();

                std::unique_lock writeLock(idsLock);

                auto existing = ids.find(name);
                if (existing != ids.end()) return false;

                ID id = nextId++;
                ids.emplace(name, id);
                names.emplace(id, std::string(name));
                store.emplace(id, MakeUnique<Pool<C>>());
                return true;
            }

            /**
             * @brief Revokes a component from the registry by type and destroys its storage.
             * @param name The name of the component.
             * @return True if the component existed in the registry, false otherwise.
             * @note This function is thread-safe.
             */
            b8 revoke(const std::string_view name);

            /**
             * @brief Revokes a component from the registry by type and destroys its storage.
             * @tparam C The component type to revoke.
             * @return True if the component existed in the registry, false otherwise.
             * @note This function is thread-safe.
             */
            template <Component C>
            b8 revoke() {
                static const std::string_view name = Reflect::reflect<C>().getType().getName();

                std::unique_lock lock(idsLock);
                auto idIt = ids.find(name);
                if (idIt == ids.end()) return false;

                const ID id = idIt->second;
                store.erase(id);
                names.erase(id);
                ids.erase(idIt);

                for (auto& [entity, archetype] : archetypes) {
                    archetype.reset(id);
                }

                return true;
            }

            /**
             * @brief Checks whether a component has been submitted.
             * @param name The name of the component.
             * @return True if the component exists in the registry, false otherwise.
             */
            b8 check(const std::string_view name);

            /**
             * @brief Checks whether a component has been submitted by type.
             * @tparam C The component type to check.
             * @return True if the component exists in the registry, false otherwise.
             */
            template <Component C>
            b8 check() const {
                return getID<C>() != INVALID_ID;
            }

            /**
             * @brief Gets the component ID for a component type.
             * @tparam C The component type to get the ID for.
             * @return The ID of the component type, or INVALID_ID if it has not been submitted.
             * @note This function is thread-safe.
             */
            template <Component C>
            [[nodiscard]] ID getID() const {
                auto it = ids.find(name);
                return it != ids.end() ? it->second : INVALID_ID;
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam C The component type to create.
             * @param entity The entity to create the component for.
             * @return The created component or nullptr.
             * @warning This function is not thread-safe.
             */
            template <Component C>
            [[nodiscard]] C* create(const Entity& entity, const T& component) {
                if (!check<C>()) return nullptr;
                ID id = getID<C>();
                Pool<T>* pool = getPool<C>();
                pool->insert(entity, component);
                archetypes[entity.getID()].set(id);
                return pool->get(entity);
            }

            /**
             * @brief Creates a new component in-place for the given entity.
             * @tparam T The component type to create.
             * @tparam ...Args The types of the arguments to forward to the component constructor.
             * @param entity The entity to create the component for.
             * @param ...args The arguments to forward to the component constructor.
             * @return The created component or nullptr.
             * @warning This function is not thread-safe.
             */
            template <Component T, typename... Args>
            [[nodiscard]] T* emplace(const Entity& entity, Args&&... args) {
                if (!check<T>()) return nullptr;
                ID id = getID<T>();
                Pool<T>* pool = getPool<T>();
                pool->emplace(entity, std::forward<Args>(args)...);
                archetypes[entity.getID()].set(id);
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
                if (!check<T>()) return;
                ID id = getID<T>();
                getPool<T>()->remove(entity);
                auto it = archetypes.find(entity.getID());
                if (it != archetypes.end()) it->second.reset(id);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity or None.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            [[nodiscard]] T* get(const Entity& entity) noexcept {
                Pool<T>* pool = getPool<T>();
                return pool ? pool->get(entity) : nullptr;
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity or None.
             * @warning This function is not thread-safe.
             */
            template <Component T>
            [[nodiscard]] const T* get(const Entity& entity) const noexcept {
                const Pool<T>* pool = getPool<T>();
                return pool ? pool->get(entity) : nullptr;
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
                const Pool<T>* pool = getPool<T>();
                return pool ? pool->contains(entity) : false;
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
             * @return The name of the component type, or an empty string for unknown IDs.
             * @warning This function is thread-safe.
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
                if (id == INVALID_ID) return nullptr;
                std::shared_lock readLock(idsLock);
                auto it = store.find(id);
                return it != store.end() ? static_cast<Pool<T>*>(it->second.get()) : nullptr;
            }

            /**
             * @brief Fetches the concrete pool for the given component type.
             * @tparam T The component type to fetch the pool for.
             * @return The pool for the given component type.
             */
            template <Component C>
            const Pool<C>* getPool() const {
                ID id = getID<C>();
                if (id == INVALID_ID) return nullptr;
                auto it = store.find(id);
                return it != store.end() ? static_cast<const Pool<C>*>(it->second.get()) : nullptr;
            }

            private:
            mutable std::shared_mutex idsLock;                                            ///< Ensure thread-safe access to the IDs map.
            std::unordered_map<ID, Unique<Storage>> store;                                ///< Storage for component pools.
            std::unordered_map<Entity::ID, BitSet<>> archetypes;                          ///< Maps entity IDs to their archetype signatures.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps component names to their IDs.
            std::unordered_map<ID, std::string> names;                                    ///< Reverse lookup.
            std::queue<ID> freeIDs;                                                       ///< Queue of free IDs for reuse.
            ID nextId = 1;                                                                ///< The next available non-null ID for a component.

            /**
             * @brief Retrieves the name of an component by its type.
             * @tparam E The type of the component.
             * @return The name of the component.
             * @note The component does not need to exist in the registry.
             */
            template <Component C>
            constexpr const std::string_view getName() {
                return Reflect::reflect<C>().getType().getName();
            }
        };
    }  // namespace Component
}  // namespace rome::core
