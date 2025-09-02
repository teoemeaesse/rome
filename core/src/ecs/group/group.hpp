#pragma once

#include "container/bitset.hpp"
#include "ecs/component/component.hpp"
#include "ecs/entity/entity.hpp"
#include "ecs/event/event.hpp"

namespace rome::core {
    struct World;

    namespace System {
        struct Descriptor;
    }

    class RM_API Group final {
        public:
        const BitSet<Component::ID> owning;   ///< The components that this group fully owns.
        const BitSet<Component::ID> partial;  ///< The components that this group partially owns.
        const BitSet<Event::ID> emits;        ///< The events this group emits.
        const BitSet<Event::ID> listens;      ///< The events this group is interested in.

        Group(const System::Descriptor& descriptor);
        ~Group() = default;
        Group(const Group&) = delete;
        Group& operator=(const Group&) = delete;
        Group(Group&&) = delete;
        Group& operator=(Group&&) = delete;

        class Registry;
        using ID = u32;

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
         * @brief Adds an entity to this group.
         * @param entity The entity to add.
         */
        void addEntity(const Entity& entity);

        /**
         * @brief Removes an entity from this group.
         * @param entity The entity to remove.
         */
        void removeEntity(const Entity& entity);

        /**
         * @brief Adds an entity to this group, assuming it fully owns the entity.
         * @param entity The entity to add.
         */
        void addEntityOwned(const Entity& entity);

        /**
         * @brief Removes an entity from this group, assuming it fully owns the entity.
         * @param entity The entity to remove.
         */
        void removeEntityOwned(const Entity& entity);

        /**
         * @brief Checks if an entity is in this group.
         * @param entity The entity to check.
         * @return True if the entity is in this group, false otherwise.
         */
        b8 contains(const Entity& entity) const noexcept;

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
         * @brief Checks if the group is empty.
         * @return True if the group is empty, false otherwise.
         */
        b8 isEmpty() const noexcept;

        private:
        const World& world;            ///< The world instance for accessing ECS data.
        std::vector<Entity> entities;  ///< The entities that match this group.
        u64 head;                      ///< The head of the group, used for iterating on owned groups.
    };
}  // namespace rome::core
