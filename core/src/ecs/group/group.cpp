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

        for (Component::ID id = 0; id < world.components.getSize(); id++) {
            if (owning.test(id))
                emit(id, '+');
            else if (partial.test(id))
                emit(id, '~');
        }

        return debug;
    }

    void Group::addEntity(const Entity& entity) {
        RM_ASSERT_MSG(!contains(entity), "Entity is already in the group");
        entities.push_back(entity);
    }

    void Group::removeEntity(const Entity& entity) {
        RM_ASSERT_MSG(contains(entity), "Entity is not in the group");
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    }

    void Group::addEntityOwned(const Entity& entity) {
        RM_ASSERT_MSG(!contains(entity), "Entity is already in the group");
        RM_ASSERT_MSG(world.components.contains(entity, owning), "Entity archetype does not match the required owning components for the group");

        entities.push_back(entity);
        head++;
    }

    void Group::removeEntityOwned(const Entity& entity) {
        RM_ASSERT_MSG(contains(entity), "Entity is not in the group");

        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
        head--;
    }

    b8 Group::contains(const Entity& entity) const noexcept { return std::find(entities.begin(), entities.end(), entity) != entities.end(); }

    const std::vector<Entity>& Group::getEntities() const noexcept { return entities; }

    u64 Group::getSize() const noexcept { return entities.size(); }

    b8 Group::isEmpty() const noexcept { return entities.empty(); }
}  // namespace rome::core
