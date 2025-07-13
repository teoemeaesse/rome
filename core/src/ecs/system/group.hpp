#pragma once

#include "container/bitset.hpp"
#include "ecs/component/registry.hpp"
#include "ecs/event/registry.hpp"

namespace rome::core {
    struct World;

    namespace System {
        struct Descriptor;

        class RM_API Group final {
            public:
            const BitSet<Component::ID> owning;   ///< The components that this group fully owns.
            const BitSet<Component::ID> partial;  ///< The components that this group partially owns.
            const BitSet<Event::ID> emits;        ///< The events this group emits.
            const BitSet<Event::ID> listens;      ///< The events this group is interested in.

            Group(const Descriptor& descriptor);
            ~Group() = default;
            Group(const Group&) = delete;
            Group& operator=(const Group&) = delete;
            Group(Group&&) = delete;
            Group& operator=(Group&&) = delete;

            /**
             * @brief Returns a debug string representation of the group.
             * @return A string representation of the group.
             */
            operator std::string() const;

            /**
             * @brief Returns a debug string representation of the group.
             * @return A string representation of the group.
             */
            std::string toString() const;

            /**
             * @brief Returns the entities currently in this group.
             * @return A vector of entities in this group.
             */
            const std::vector<Entity>& getEntities() const noexcept;

            /**
             * @brief Returns the number of entities in this group.
             * @return The number of entities in this group.
             */
            u64 getSize() const noexcept;

            /**
             * @brief Returns the number of components this group fully owns.
             * @return The number of components this group fully owns.
             */
            u64 getOwned() const noexcept;

            /**
             * @brief Checks if the group is empty.
             * @return True if the group is empty, false otherwise.
             */
            b8 isEmpty() const noexcept;

            private:
            const World& world;            ///< The world instance for accessing ECS data.
            std::vector<Entity> entities;  ///< The entities that match this group.
        };

    }  // namespace System
}  // namespace rome::core