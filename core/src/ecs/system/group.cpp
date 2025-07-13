#include "ecs/system/group.hpp"

#include "ecs/system/descriptor.hpp"

namespace rome::core {
    namespace System {
        Group::Group(const Descriptor& descriptor)
            : owning(descriptor.requireFull ? descriptor.writes | descriptor.reads : descriptor.writes),
              partial(descriptor.allowPartial ? descriptor.reads - owning : BitSet<Component::ID>{}),
              emits(descriptor.emits),
              listens(descriptor.listens),
              world(descriptor.world) {}

        Group::operator std::string() const { return toString(); }

        std::string Group::toString() const {
            std::string debug;
            auto emit = [&](Component::ID id, char prefix) {
                if (!debug.empty()) debug += ", ";
                debug += prefix;
                debug += world.components.getName(id);
            };

            for (Component::ID id = 0; id < world.components.getCount(); id++) {
                if (owning.test(id))
                    emit(id, '+');
                else if (partial.test(id))
                    emit(id, '~');
            }

            return debug;
        }

        const std::vector<Entity>& Group::getEntities() const noexcept { return entities; }

        u64 Group::getSize() const noexcept { return entities.size(); }

        u64 Group::getOwned() const noexcept { return owning.count(); }

        b8 Group::isEmpty() const noexcept { return entities.empty(); }
    }  // namespace System
}  // namespace rome::core