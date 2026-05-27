#include "rm/ecs/component/registry.hpp"

namespace rome::core {
    namespace Component {
        u32 Registry::getSize() const noexcept { return store.size(); }

        b8 Registry::submit(const std::string& name){return }

        b8 Registry::contains(const Entity& entity, const BitSet<>& components) noexcept {
            auto it = archetypes.find(entity.getIndex());
            if (it == archetypes.end()) return false;
            return (it->second & components) == components;
        }

        std::string Registry::getName(ID id) const {
            std::shared_lock lock(idsLock);
            auto it = names.find(id);
            return it != names.end() ? it->second : std::string{};
        }
    }  // namespace Component
}  // namespace rome::core
