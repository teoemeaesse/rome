#pragma once

#include "rm/ecs/entity/entity.hpp"

namespace rome::core {
    /**
     * @brief A registry to manage the lifecycle of entities.
     * @warning This class is not thread-safe.
     */
    class Entity::Registry final {
        public:
        class Iterator;
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
         * @brief Gets the live entity stored at a slot.
         * @param index The entity slot index.
         * @return The entity stored at the slot.
         */
        [[nodiscard]] Entity get(u64 index) const;

        /**
         * @brief Checks if an entity is alive.
         * @param entity The entity to check.
         * @return True if the entity is alive, false otherwise.
         */
        [[nodiscard]] b8 isAlive(Entity entity) const;

        /**
         * @brief Returns the number of slots in the entity pool.
         * @return The number of slots in the entity pool, including reusable holes.
         */
        u64 getCapacity() const noexcept;

        /* Non-const iterator interfaces */
        Iterator begin() const;
        Iterator end() const;

        class Iterator final {
            public:
            Iterator(const Registry& registry, u64 index);

            bool operator!=(const Iterator& iter) const { return index != iter.index; }
            Iterator& operator++();
            Entity operator*() const;

            private:
            void advance();

            const Registry& registry;
            u64 index;
        };

        private:
        std::vector<u64> entities;  ///< The entity pool.
        u64 next = 0;               ///< The next available entity index.
        u64 available = 0;          ///< The number of available entities.
    };
}  // namespace rome::core
