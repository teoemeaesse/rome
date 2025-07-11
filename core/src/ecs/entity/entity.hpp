#pragma once

#include "prelude.hpp"

namespace rome::core {
    class RM_API Entity {
        public:
        /**
         * @brief The unique identifier for an entity.
         * 48 most significant bits for the index, 16 least significant bits for the version.
         */
        using ID = u64;

        ~Entity() = default;

        inline b8 operator==(const Entity& other) const { return id == other.id; }
        inline b8 operator!=(const Entity& other) const { return id != other.id; }

        inline u64 getIndex() const { return getIndex(id); }
        inline u64 getVersion() const { return getVersion(id); }

        class Registry;

        private:
        ID id;  ///< The entity ID.

        Entity(ID id);

        static inline u64 getIndex(ID id) { return id >> 16; }
        static inline u64 getVersion(ID id) { return id & 0xFFFF; }
        static inline void setIndex(ID& id, u64 index) { id = (id & 0x000000000000FFFFULL) | (index << 16); }
        static inline void setVersion(ID& id, u64 version) { id = (id & 0xFFFFFFFFFFFF0000ULL) | (version & 0xFFFF); }
    };
}  // namespace rome::core