#include "ecs/entity/registry.hpp"

namespace iodine::core {

    Entity::Registry::Registry() : next(0), available(0) {}

    Entity Entity::Registry::create() {
        std::unique_lock lock(entitiesLock);

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
        std::unique_lock lock(entitiesLock);

        const u64 index = getIndex(entity.id);
        setIndex(entities[index], next);
        setVersion(entities[index], getVersion(entities[index]) + 1);
        next = index;
        available++;
    }

    b8 Entity::Registry::isAlive(Entity entity) const {
        std::shared_lock lock(entitiesLock);
        return getVersion(entities[getIndex(entity.id)]) == getVersion(entity.id);
    }
}  // namespace iodine::core