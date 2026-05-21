#include "rm/ecs/entity/registry.hpp"

namespace rome::core {
    Entity Entity::Registry::create() {
        if (available == 0) {
            const u64 index = entities.size();
            entities.emplace_back(static_cast<ID>(index << 16));
            return Entity(entities.back());
        }

        const u64 index = next;
        next = getIndex(entities[index]);
        setIndex(entities[index], index);
        available--;
        return Entity(entities[index]);
    }

    void Entity::Registry::destroy(Entity entity) {
        if (!isAlive(entity)) return;
        const u64 index = getIndex(entity.id);
        setIndex(entities[index], next);
        setVersion(entities[index], getVersion(entities[index]) + 1);
        next = index;
        available++;
    }

    b8 Entity::Registry::isAlive(Entity entity) const {
        const u64 index = getIndex(entity.id);

        return index < entities.size() && getVersion(entities[index]) == getVersion(entity.id);
    }
}  // namespace rome::core
