#pragma once

#include <shared_mutex>

#include "ecs/entity/entity.hpp"

namespace rome::core {

    /**
     * @brief A registry to manage the creation and destruction of entities.
     * @warning This registry is not thread-safe.
     */
    class RM_API Entity::Registry final {
        public:
        Registry() = default;
        ~Registry() = default;

        /**
         * @brief Creates a new entity.
         * @return The ID of the new entity.
         * @warning This function is not thread-safe.
         */
        Entity create();

        /**
         * @brief Destroys an entity.
         * @param entity The entity to destroy.
         * @warning This function is not thread-safe.
         */
        void destroy(Entity entity);

        /**
         * @brief Checks if an entity is alive.
         * @param entity The entity to check.
         * @return True if the entity is alive, false otherwise.
         * @warning This function is not thread-safe.
         */
        b8 isAlive(Entity entity) const;

        private:
        std::vector<u64> entities;  ///< The entity pool.
        u64 next;                   ///< The next available entity index.
        u64 available;              ///< The number of available entities.
    };
}  // namespace rome::core