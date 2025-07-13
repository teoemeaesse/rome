#include "ecs/system/descriptor.hpp"

namespace rome::core {
    namespace System {
        Builder::Builder(const std::string& name, const World& world)
            : descriptor{world, name, nullptr, {}, {}, {}, {}, false, false, true}, world(world) {}

        Builder& Builder::emits(std::initializer_list<Event::ID> events) {
            descriptor.emits = BitSet<Event::ID>::create(events);
            return *this;
        }

        Builder& Builder::listens(std::initializer_list<Event::ID> events) {
            descriptor.listens = BitSet<Event::ID>::create(events);
            return *this;
        }

        Builder& Builder::requireFull(b8 value) {
            descriptor.requireFull = value;
            return *this;
        }

        Builder& Builder::allowPartial(b8 value) {
            descriptor.allowPartial = value;
            return *this;
        }
    }  // namespace System
}  // namespace rome::core