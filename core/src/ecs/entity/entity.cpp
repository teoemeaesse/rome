#include "ecs/entity/entity.hpp"

namespace iodine::core {
    Entity::Entity(ID id) : id(id) {}

    Entity::Registry::Registry() : next(0), available(0) {}

    Entity& Entity::Registry::create() {
        if (available == 0) {
            const u64 index = entities.size();
            entities.emplace_back(static_cast<ID>(index << 16));
            return entities.back();
        }

        const u64 index = next;
        next = getIndex(entities[index].id);
        setIndex(entities[index].id, index);
        available--;
        return entities[index];
    }

    void Entity::Registry::destroy(Entity& entity) {
        const u64 index = getIndex(entity.id);
        setIndex(entities[index].id, next);
        setVersion(entities[index].id, getVersion(entities[index].id) + 1);
        next = index;
        available++;
    }
}  // namespace iodine::core