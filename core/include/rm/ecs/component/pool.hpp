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
        class RM_API Storage {
            public:
            virtual ~Storage() = default;
        };

        /**
         * @brief Manages the pool of a component type.
         * @tparam T The component type to manage.
         */
        template <Component T>
        class RM_API Pool final : public Storage {
            public:
            Pool() = default;
            ~Pool() = default;
            Pool(const Pool& other) = delete;
            Pool(Pool&& other) noexcept = default;
            Pool& operator=(const Pool& other) = delete;
            Pool& operator=(Pool&& other) noexcept = default;

            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return A pointer to the component for the given entity.
             */
            [[nodiscard]] T* get(const Entity& entity) noexcept { return entities[entity.getIndex()]; }

            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return A const pointer to the component for the given entity.
             */
            [[nodiscard]] const T* get(const Entity& entity) const noexcept { return entities[entity.getIndex()]; }

            /**
             * @brief Inserts the component for the given entity.
             * @param entity The entity to insert the component for.
             * @param component The component to insert.
             */
            void insert(const Entity& entity, const T& component) {
                if (entities.contains(entity.getIndex())) {
                    RM_WARN("Entity already has component of type: %s", getType().getName().c_str());
                    return;
                }
                entities.emplace(entity.getIndex(), component);
            }

            /**
             * @brief Inserts the component for the given entity.
             * @tparam ...Args Constructor arguments for the component.
             * @param entity The entity to insert the component for.
             * @param component The component to insert.
             */
            template <typename... Args>
            void emplace(const Entity& entity, Args&&... args) {
                if (entities.contains(entity.getIndex())) {
                    RM_WARN("Entity already has component of type: %s", getType().getName().c_str());
                    return;
                }
                entities.emplace(entity.getIndex(), T(std::forward<Args>(args)...));
            }

            /**
             * @brief Removes the component for the given entity.
             * @param entity The entity to remove the component for.
             */
            void remove(const Entity& entity) {
                if (!entities.contains(entity.getIndex())) {
                    RM_WARN("Entity does not have component of type: %s", getType().getName().c_str());
                    return;
                }
                entities.erase(entity.getIndex());
            }

            /**
             * @brief Checks if the entity has this component.
             * @param entity The entity to check.
             * @return True if the entity has this component, false otherwise.
             */
            b8 contains(const Entity& entity) const noexcept { return entities.contains(entity.getIndex()); }

            /**
             * @brief Retrieves a contiguous data pointer and the size of the pool.
             * @return A pair containing a pointer to the start of the block and the size of the pool.
             */
            std::pair<T*, u64> getData() noexcept { return entities.getData(); }

            /**
             * @brief Retrieves a contiguous data pointer and the size of the pool.
             * @return A pair containing a pointer to the start of the block and the size of the pool.
             */
            const std::pair<const T*, u64> getData() const noexcept { return entities.getData(); }

            /**
             * @brief Gets the reflected type for this pool's component type.
             * @return The reflected type for this pool's component type.
             */
            Type& getType() const {
                static Type& type = Reflect::reflect<T>();
                return type;
            }

            inline std::vector<T>::iterator begin() noexcept { return entities.begin(); }
            inline std::vector<T>::iterator end() noexcept { return entities.end(); }

            inline std::vector<T>::const_iterator begin() const noexcept { return entities.begin(); }
            inline std::vector<T>::const_iterator end() const noexcept { return entities.end(); }

            private:
            SparseSet<T> entities;  ///< The entities with this component.
        };
    }  // namespace Component
}  // namespace rome::core
