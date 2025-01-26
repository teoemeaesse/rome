#pragma once

#include <vector>

#include "prelude.hpp"

namespace iodine::core {
    class IO_API Entity {
        /**
         * @brief The unique identifier for an entity.
         * 48 most significant bits for the index, 16 least significant bits for the version.
         */
        using ID = u64;

        public:
        Entity(ID id);
        ~Entity() = default;

        /**
         * @brief Manages creation and destruction of entities.
         */
        class Registry;

        private:
        ID id;  // The entity ID.

        static inline void setIndex(ID& id, u64 index) { id = (id & 0x000000000000FFFF) | (index << 16); }
        static inline void setVersion(ID& id, u64 version) { id = (id & 0xFFFFFFFFFFFF0000) | version; }
        static inline u64 getIndex(ID id) { return static_cast<u64>(id) & 0xFFFFFFFFFFFF0000; }
        static inline u64 getVersion(ID id) { return static_cast<u64>(id) & 0x000000000000FFFF; }
    };

    /**
     * @brief Manages creation and destruction of entities.
     */
    class IO_API Entity::Registry {
        public:
        Registry();
        ~Registry() = default;

        /**
         * @brief Creates a new entity.
         * @return The ID of the new entity.
         */
        Entity& create();

        /**
         * @brief Destroys an entity.
         * @param entity The entity to destroy.
         */
        void destroy(Entity& entity);

        private:
        std::vector<Entity> entities;  // The entity pool.
        u64 next;                      // The next available entity index.
        u64 available;                 // The number of available entities.
    };
}  // namespace iodine::core