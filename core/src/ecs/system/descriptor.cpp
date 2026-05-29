#include "rm/ecs/system/descriptor.hpp"

namespace rome::core {
    namespace System {
        Builder::Builder(const std::string_view name, World& world)
            : descriptor{world, std::string(name), nullptr, {}, {}, {}, {}, false, false, true}, world(world) {}

        Builder& Builder::emits(std::initializer_list<Event::ID> events) {
            descriptor.emits = BitSet<>::create(events);
            return *this;
        }

        Builder& Builder::listens(std::initializer_list<Event::ID> events) {
            descriptor.listens = BitSet<>::create(events);
            return *this;
        }

        Builder& Builder::requireFull() {
            descriptor.requireFull = true;
            return *this;
        }

        Builder& Builder::allowPartial() {
            descriptor.allowPartial = true;
            return *this;
        }
    }  // namespace System
}  // namespace rome::core
