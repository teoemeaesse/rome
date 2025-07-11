#include "ecs/system/system.hpp"

namespace rome::core::System {
    Group Descriptor::getGroup() const {
        BitSet<Component::ID> owning = writes;
        BitSet<Component::ID> partial;

        if (requireFull) {
            owning |= reads;
        }
        if (allowPartial) {
            partial = reads - owning;
        }
        return Group(owning, partial, components);
    }

    Builder::Builder(const std::string& name, Component::Registry& components, Event::Registry& events)
        : descriptor{components, name, components.count(), nullptr, {}, {}, {}, {}, false, false}, components(components), events(events) {}

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

    Descriptor Builder::build() { return descriptor; }

}  // namespace rome::core::System