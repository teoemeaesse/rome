#include "rm/ecs/component/registry.hpp"

#include "rm/ecs/component/component.hpp"

namespace rome::core {
    namespace Component {
        u32 Registry::getSize() const noexcept { return store.size(); }

        b8 Registry::revoke(const std::string_view name) {
            auto idIt = ids.find(name);
            if (idIt == ids.end()) return false;

            const ID id = idIt->second;
            store.erase(id);
            names.erase(id);
            ids.erase(idIt);

            for (auto& [entity, archetype] : archetypes) {
                archetype.reset(id);
            }

            return true;
        }

        b8 Registry::check(const std::string_view name) const { return getID(name) != INVALID_ID; }

        ID Registry::getID(const std::string_view name) const {
            auto it = ids.find(name);
            return it != ids.end() ? it->second : INVALID_ID;
        }

        b8 Registry::contains(Entity entity, const BitSet<>& components) noexcept {
            auto it = archetypes.find(entity.getIndex());
            if (it == archetypes.end()) return false;
            return (it->second & components) == components;
        }

        std::string_view Registry::getName(ID id) const {
            auto it = names.find(id);
            return it != names.end() ? it->second : INVALID_NAME;
        }
    }  // namespace Component
}  // namespace rome::core
