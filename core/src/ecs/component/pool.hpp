#pragma once

#include <concepts>

#include "container/sparse_set.hpp"
#include "debug/log.hpp"
#include "ecs/component/storage.hpp"
#include "ecs/entity/entity.hpp"

namespace rome::core {
    namespace Component {
        /**
         * @brief Manages the pool of a component type.
         * @tparam T The component type to manage.
         */
        template <Component T>
        class RM_API Pool : public Storage {
            public:
            Pool() : type(Reflect::reflect<T>().getType()) {}
            ~Pool() = default;
            Pool(const Pool& other) = delete;
            Pool(Pool&& other) noexcept = default;
            Pool& operator=(const Pool& other) = delete;
            Pool& operator=(Pool&& other) noexcept = default;

            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            T& get(const Entity& entity) {
                RM_ASSERT_MSG(entities.contains(entity.getIndex()), "Entity does not have component T");
                return entities[entity.getIndex()];
            }

            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            const T& get(const Entity& entity) const {
                RM_ASSERT_MSG(entities.contains(entity.getIndex()), "Entity does not have component T");
                return entities[entity.getIndex()];
            }

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
             * @brief Gets the reflected type for this pool's component type.
             * @return The reflected type for this pool's component type.
             */
            Type& getType() const {
                static Type& type = Reflect::reflect<T>();
                return type;
            }

            inline std::vector<T>::iterator begin() { return entities.begin(); }
            inline std::vector<T>::iterator end() { return entities.end(); }

            inline std::vector<T>::const_iterator begin() const { return entities.begin(); }
            inline std::vector<T>::const_iterator end() const { return entities.end(); }

            private:
            SparseSet<T> entities;  ///< The entities with this component.
            Type& type;             ///< The reflected type for this component.
        };
    }  // namespace Component
}  // namespace rome::core