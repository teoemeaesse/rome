#pragma once

#include "rm/container/bitset.hpp"
#include "rm/ecs/component/pool.hpp"

namespace rome::core {
    namespace Component {
        /**
         * @brief Manages the lifecycle of components within the ECS.
         * @warning Components must implement reflection and a copy-constructor to be submitted.
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
             * @brief Submits a component to the registry by type.
             * @tparam C The component type to submit.
             * @return True if the component did not exist in the registry, false otherwise.
             */
            template <Component C>
            b8 submit() {
                auto existing = ids.find(getName<C>());
                if (existing != ids.end()) return false;

                ID id = nextId++;
                ids.emplace(getName<C>(), id);
                names.emplace(id, std::string(getName<C>()));
                store.emplace(id, MakeUnique<Pool<C>>());
                return true;
            }

            /**
             * @brief Revokes a component from the registry and destroys its storage.
             * @param name The name of the component.
             * @return True if the component existed in the registry, false otherwise.
             */
            b8 revoke(const std::string_view name);

            /**
             * @brief Revokes a component from the registry by type and destroys its storage.
             * @tparam C The component type to revoke.
             * @return True if the component existed in the registry, false otherwise.
             */
            template <Component C>
            b8 revoke() {
                return revoke(getName<C>());
            }

            /**
             * @brief Checks whether a component exists in the registry.
             * @param name The name of the component.
             * @return True if the component exists in the registry, false otherwise.
             */
            [[nodiscard]] b8 check(const std::string_view name);

            /**
             * @brief Checks whether a component exists in the registry by type.
             * @tparam C The component type to check.
             * @return True if the component exists in the registry, false otherwise.
             */
            template <Component C>
            [[nodiscard]] b8 check() const {
                return getID<C>() != INVALID_ID;
            }

            /**
             * @brief Gets the component ID for a component by name.
             * @tparam name The component name.
             * @return The ID of the component type, or INVALID_ID if it has not been submitted.
             */
            [[nodiscard]] ID getID(const std::string_view name) const;

            /**
             * @brief Gets the component ID for a component by type.
             * @tparam C The component type.
             * @return The ID of the component type, or INVALID_ID if it has not been submitted.
             */
            template <Component C>
            [[nodiscard]] ID getID() const {
                return getID(getName<C>());
            }

            /**
             * @brief Inserts a component for an entity.
             * @tparam C The component type.
             * @param entity The entity to insert the component.
             * @param component The component to insert.
             * @return A pointer to the component or nullptr.
             */
            template <Component C>
            [[nodiscard]] C* insert(Entity entity, const C& component) {
                if (!check<C>()) return nullptr;
                ID id = getID<C>();
                Pool<C>* pool = getPool<C>();
                pool->insert(entity, component);
                archetypes[entity.getID()].set(id);
                return pool->get(entity);
            }

            /**
             * @brief Creates a new component in-place for an entity.
             * @tparam C The component type.
             * @param entity The entity to insert the component.
             * @param ...args The arguments to forward to the component constructor.
             * @return A pointer to the component or nullptr.
             */
            template <Component C, typename... Args>
            [[nodiscard]] C* emplace(Entity entity, Args&&... args) {
                if (!check<C>()) return nullptr;
                ID id = getID<C>();
                Pool<C>* pool = getPool<C>();
                pool->emplace(entity, std::forward<Args>(args)...);
                archetypes[entity.getID()].set(id);
                return pool->get(entity);
            }

            /**
             * @brief Removes a component from an entity.
             * @tparam C The component type.
             * @param entity The entity to remove the component.
             */
            template <Component C>
            void remove(Entity entity) {
                if (!check<C>()) return;
                ID id = getID<C>();
                getPool<C>()->remove(entity);
                auto it = archetypes.find(entity.getID());
                if (it != archetypes.end()) it->second.reset(id);
            }

            /**
             * @brief Gets a component from an entity.
             * @tparam C The component type.
             * @param entity The entity to get the component.
             * @return The component for the given entity or nullptr.
             */
            template <Component C>
            [[nodiscard]] C* get(Entity entity) noexcept {
                Pool<C>* pool = getPool<C>();
                return pool ? pool->get(entity) : nullptr;
            }

            /**
             * @brief Gets a component from an entity.
             * @tparam C The component type.
             * @param entity The entity to get the component.
             * @return The component for the given entity or nullptr.
             */
            template <Component C>
            [[nodiscard]] const C* get(Entity entity) const noexcept {
                const Pool<C>* pool = getPool<C>();
                return pool ? pool->get(entity) : nullptr;
            }

            /**
             * @brief Checks if an entity has a component.
             * @tparam C The component type.
             * @param entity The entity to check.
             * @return True if the entity has the component, false otherwise.
             */
            template <Component C>
            b8 has(Entity entity) const noexcept {
                const Pool<C>* pool = getPool<C>();
                return pool ? pool->contains(entity) : false;
            }

            /**
             * @brief Checks if an entity's archetype contains all the given components.
             * @param entity The entity to check.
             * @param components The set of components to check against.
             * @return True if the entity's archetype contains every component, false otherwise.
             * @note The archetype may have other components as well.
             */
            b8 contains(Entity entity, const BitSet<>& components) noexcept;

            /**
             * @brief Calculates the total number of component types in the registry.
             * @return The total number of component types.
             */
            u32 getSize() const noexcept;

            /**
             * @brief Fetches the concrete pool for a component by type.
             * @tparam C The component type.
             * @return The component pool or nullptr.
             */
            template <Component C>
            Pool<C>* getPool() {
                ID id = getID<C>();
                if (id == INVALID_ID) return nullptr;
                auto it = store.find(id);
                return it != store.end() ? static_cast<Pool<C>*>(it->second.get()) : nullptr;
            }

            /**
             * @brief Fetches the concrete pool for a component by type.
             * @tparam C The component type.
             * @return The component pool or nullptr.
             */
            template <Component C>
            const Pool<C>* getPool() const {
                ID id = getID<C>();
                if (id == INVALID_ID) return nullptr;
                auto it = store.find(id);
                return it != store.end() ? static_cast<const Pool<C>*>(it->second.get()) : nullptr;
            }

            /**
             * @brief Retrieves the name of a component by ID.
             * @param id The id of the component.
             * @return The name of the component.
             */
            [[nodiscard]] const std::string_view getName(ID id) const;

            /**
             * @brief Retrieves the name of an component by type at compile time.
             * @tparam C The type of the component.
             * @return The name of the component.
             * @note The component does not need to exist in the registry.
             */
            template <Component C>
            [[nodiscard]] constexpr const std::string_view getName() const {
                return Reflect::getName<C>();
            }

            private:
            std::unordered_map<ID, Unique<Storage>> store;                                ///< Storage for component pools.
            std::unordered_map<Entity::ID, BitSet<>> archetypes;                          ///< Maps entity IDs to their archetype signatures.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps component names to their IDs.
            std::unordered_map<ID, std::string> names;                                    ///< Reverse lookup.
            std::queue<ID> freeIDs;                                                       ///< Queue of free IDs for reuse.
            ID nextId = 1;                                                                ///< The next available non-null ID for a component.
        };
    }  // namespace Component
}  // namespace rome::core
