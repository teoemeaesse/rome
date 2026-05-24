#pragma once

#include "rm/ecs/entity/entity.hpp"

namespace rome::core {
    /**
     * @brief A registry to manage the creation and destruction of entities.
     * @warning This registry is not thread-safe.
     */
    class RM_API Entity::Registry final {
        public:
        Registry();
        ~Registry() = default;

        /**
         * @brief Creates a new entity.
         * @return The ID of the new entity.
         * @warning This function is not thread-safe.
         */
        Entity create();

        /**
         * @brief Returns the number of slots in the entity pool.
         * @return The number of slots in the entity pool, including reusable holes.
         * @warning This function is not thread-safe.
         */
        u64 getCapacity() const noexcept;

        /**
         * @brief Returns true if the slot currently stores a live entity.
         * @param index The entity slot index.
         * @return True if the slot stores a live entity, false otherwise.
         * @warning This function is not thread-safe.
         */
        b8 isOccupied(u64 index) const noexcept;

        /**
         * @brief Gets the live entity stored at a slot.
         * @param index The entity slot index.
         * @return The entity stored at the slot.
         * @warning This function is not thread-safe.
         */
        Entity get(u64 index) const;

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
        u64 next = 0;               ///< The next available entity index.
        u64 available = 0;          ///< The number of available entities.
    };
}  // namespace rome::core
