#include "rm/ecs/component/registry.hpp"

namespace rome::core {
    namespace Component {
        u32 Registry::getSize() const noexcept { return store.size(); }

        b8 Registry::contains(const Entity& entity, const BitSet<>& components) noexcept {
            return (archetypes[entity.getIndex()] & components) == components;
        }

        std::string Registry::getName(ID id) const {
            auto it = names.find(id);
            if (it != names.end()) {
                return it->second;
            }
            return "";
        }
    }  // namespace Component
}  // namespace rome::core
