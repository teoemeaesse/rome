#pragma once

#include <shared_mutex>

#include "ecs/entity/entity.hpp"

namespace rome::core {

    /**
     * @brief Manages creation and destruction of entities.
     */
    class RM_API Entity::Registry {
        public:
        Registry();
        ~Registry() = default;

        /**
         * @brief Creates a new entity.
         * @return The ID of the new entity.
         */
        Entity create();

        /**
         * @brief Destroys an entity.
         * @param entity The entity to destroy.
         */
        void destroy(Entity entity);

        /**
         * @brief Checks if an entity is alive.
         * @param entity The entity to check.
         * @return True if the entity is alive, false otherwise.
         */
        b8 isAlive(Entity entity) const;

        private:
        mutable std::shared_mutex entitiesLock;  ///< Mutex for thread-safe access to the entity pool.
        std::vector<u64> entities;               ///< The entity pool.
        u64 next;                                ///< The next available entity index.
        u64 available;                           ///< The number of available entities.
    };
}  // namespace rome::core