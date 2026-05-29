#pragma once

#include "rm/container/sparse_set.hpp"
#include "rm/debug/log.hpp"
#include "rm/ecs/component/component.hpp"
#include "rm/ecs/entity/entity.hpp"

namespace rome::core {
    namespace Component {
        /**
         * @brief Acts as an interface for the storage of components.
         */
        class Storage {
            public:
            virtual ~Storage() = default;
        };

        /**
         * @brief Manages the pool of a component type.
         * @tparam C The component type to manage.
         * @warning This class is not thread-safe.
         */
        template <Component C>
        class Pool final : public Storage {
            public:
            Pool() = default;
            ~Pool() = default;
            Pool(const Pool& other) = delete;
            Pool(Pool&& other) noexcept = default;
            Pool& operator=(const Pool& other) = delete;
            Pool& operator=(Pool&& other) noexcept = default;

            /**
             * @brief Gets the component for an entity.
             * @param entity The entity to get the component.
             * @return A pointer to the component.
             */
            [[nodiscard]] C* get(Entity entity) noexcept { return entities[entity.getID()]; }

            /**
             * @brief Gets the component for an entity.
             * @param entity The entity to get the component.
             * @return A const pointer to the component.
             */
            [[nodiscard]] const C* get(Entity entity) const noexcept { return entities[entity.getID()]; }

            /**
             * @brief Inserts a component for an entity.
             * @param entity The entity to insert a component.
             * @param component The component to insert.
             */
            void insert(Entity entity, const C& component) {
                if (entities.contains(entity.getID())) {
                    RM_WARN("Entity already has component of type: %s", std::string(Reflect::getName<C>()).c_str());
                    return;
                }
                entities.insert(entity.getID(), component);
            }

            /**
             * @brief Inserts a component in-place for an entity.
             * @param entity The entity to insert the component.
             * @param ...args Constructor arguments for the component.
             * @param component The component to insert.
             */
            template <typename... Args>
            void emplace(Entity entity, Args&&... args) {
                if (entities.contains(entity.getID())) {
                    RM_WARN("Entity already has component of type: %s", std::string(Reflect::getName<C>()).c_str());
                    return;
                }
                entities.emplace(entity.getID(), C(std::forward<Args>(args)...));
            }

            /**
             * @brief Removes a component from an entity.
             * @param entity The entity to remove the component.
             */
            void remove(Entity entity) {
                if (!entities.contains(entity.getID())) {
                    RM_WARN("Entity does not have component of type: %s", std::string(Reflect::getName<C>()).c_str());
                    return;
                }
                entities.erase(entity.getID());
            }

            /**
             * @brief Checks if an entity has a component.
             * @param entity The entity to check.
             * @return True if the entity has this component, false otherwise.
             */
            [[nodiscard]] b8 contains(Entity entity) const noexcept { return entities.contains(entity.getID()); }

            /**
             * @brief Retrieves a contiguous data pointer and the size of the pool.
             * @return A pair containing a pointer to the start of the block and the size of the pool.
             */
            [[nodiscard]] std::pair<C*, u64> getData() noexcept { return entities.getData(); }

            /**
             * @brief Retrieves a contiguous data pointer and the size of the pool.
             * @return A pair containing a const pointer to the start of the block and the size of the pool.
             */
            [[nodiscard]] const std::pair<const C*, u64> getData() const noexcept { return entities.getData(); }

            /**
             * @brief Gets the reflected type for this pool's component type.
             * @return The reflected type.
             */
            Type& getType() const {
                static Type& type = Reflect::reflect<C>();
                return type;
            }

            inline std::vector<C>::iterator begin() noexcept { return entities.begin(); }
            inline std::vector<C>::iterator end() noexcept { return entities.end(); }

            inline std::vector<C>::const_iterator begin() const noexcept { return entities.begin(); }
            inline std::vector<C>::const_iterator end() const noexcept { return entities.end(); }

            private:
            SparseSet<C> entities;  ///< The entities with this component.
        };
    }  // namespace Component
}  // namespace rome::core
