#include "rm/ecs/group/group.hpp"

#include "rm/ecs/system/descriptor.hpp"

namespace rome::core {
    Group::Group(const System::Descriptor& descriptor)
        : owning(descriptor.requireFull ? descriptor.writes | descriptor.reads : descriptor.writes),
          partial(descriptor.allowPartial ? descriptor.reads - owning : BitSet<>{}),
          emits(descriptor.emits),
          listens(descriptor.listens),
          world(descriptor.world),
          entities(),
          head(0) {
        RM_ASSERT_MSG(!owning.none() || !partial.none(), "A group must own at least one component, or allow partial ownership");
    }

    Group::operator std::string() const { return toString(); }

    std::string Group::toString() const {
        std::string debug;
        auto emit = [&](Component::ID id, char prefix) {
            if (!debug.empty()) debug += ", ";
            debug += prefix;
            debug += world.components.getName(id);
        };

        for (Component::ID id = 1; id <= world.components.getSize(); id++) {
            if (owning.test(id))
                emit(id, '+');
            else if (partial.test(id))
                emit(id, '~');
        }

        return debug;
    }

    void Group::addEntity(Entity entity) {
        RM_ASSERT_MSG(!contains(entity), "Entity is already in the group");
        entities.insert(entity.getIndex(), entity);
    }

    void Group::removeEntity(Entity entity) {
        RM_ASSERT_MSG(contains(entity), "Entity is not in the group");
        entities.erase(entity.getIndex());
    }

    void Group::addEntityOwned(Entity entity) {
        RM_ASSERT_MSG(!contains(entity), "Entity is already in the group");
        RM_ASSERT_MSG(matches(entity), "Entity archetype does not match the group");

        entities.insert(entity.getIndex(), entity);
        head++;
    }

    void Group::removeEntityOwned(Entity entity) {
        RM_ASSERT_MSG(contains(entity), "Entity is not in the group");

        entities.erase(entity.getIndex());
        head--;
    }

    b8 Group::contains(Entity entity) const noexcept {
        const Entity* stored = entities[entity.getIndex()];
        return stored != nullptr && *stored == entity;
    }

    b8 Group::matches(Entity entity) const noexcept { return world.components.contains(entity, owning | partial); }

    const SparseSet<Entity>& Group::getEntities() const noexcept { return entities; }

    u64 Group::getSize() const noexcept { return entities.getSize(); }

    b8 Group::isEmpty() const noexcept { return entities.getSize() == 0; }
}  // namespace rome::core
