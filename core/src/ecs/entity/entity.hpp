#pragma once

#include <vector>

#include "prelude.hpp"

namespace iodine::core {
    class IO_API Entity {
        public:
        /**
         * @brief The unique identifier for an entity.
         * 48 most significant bits for the index, 16 least significant bits for the version.
         */
        using ID = u64;

        Entity(ID id);
        ~Entity() = default;

        inline b8 operator==(const Entity& other) const { return id == other.id; }
        inline b8 operator!=(const Entity& other) const { return id != other.id; }

        inline u64 getIndex() const { return getIndex(id); }
        inline u64 getVersion() const { return getVersion(id); }

        class Registry;

        private:
        ID id;  // The entity ID.

        static inline u64 getIndex(ID id) { return id >> 16; }
        static inline u64 getVersion(ID id) { return id & 0xFFFF; }
        static inline void setIndex(ID& id, u64 index) { id = (id & 0x000000000000FFFFULL) | (index << 16); }
        static inline void setVersion(ID& id, u64 version) { id = (id & 0xFFFFFFFFFFFF0000ULL) | (version & 0xFFFF); }
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
        std::vector<u64> entities;  // The entity pool.
        u64 next;                   // The next available entity index.
        u64 available;              // The number of available entities.
    };
}  // namespace iodine::core