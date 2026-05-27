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

    Entity Entity::Registry::get(u64 index) const {
        if (index >= entities.size()) {
            std::string msg = "Entity at index " + std::to_string(index) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }

        Entity entity(entities[index]);
        if (isAlive(entity)) return entity;

        std::string msg = "Entity at index " + std::to_string(index) + " not found";
        THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
    }

    Entity::Registry::Iterator Entity::Registry::begin() const { return Iterator{*this, 0}; }

    Entity::Registry::Iterator Entity::Registry::end() const { return Iterator{*this, entities.size()}; }

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

        return entity.id != Entity::INVALID_ID && index < entities.size() && entities[index] == entity.id;
    }

    Entity::Registry::Iterator::Iterator(const Registry& registry, u64 index) : registry(registry), index(index) { advance(); }

    Entity::Registry::Iterator& Entity::Registry::Iterator::operator++() {
        index++;
        advance();
        return *this;
    }

    Entity Entity::Registry::Iterator::operator*() const { return Entity(registry.entities[index]); }

    void Entity::Registry::Iterator::advance() {
        while (index < registry.entities.size()) {
            const Entity entity(registry.entities[index]);
            if (registry.isAlive(entity)) return;
            index++;
        }
    }
}  // namespace rome::core
