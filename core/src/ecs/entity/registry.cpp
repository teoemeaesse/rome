#include "rm/ecs/entity/registry.hpp"

#include "rm/debug/exception.hpp"

namespace rome::core {
    Entity::Registry::Registry() : entities{Entity::INVALID_ID} {}

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

    u64 Entity::Registry::getCapacity() const noexcept { return entities.size(); }

    b8 Entity::Registry::isOccupied(u64 index) const noexcept { return index < entities.size() && getIndex(entities[index]) == index; }

    Entity Entity::Registry::get(u64 index) const {
        if (!isOccupied(index)) {
            std::string msg = "Entity at index " + std::to_string(index) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }

        Entity entity(entities[index]);
        if (isAlive(entity)) return entity;

        std::string msg = "Entity at index " + std::to_string(index) + " not found";
        THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
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
