#pragma once

#include "container/bitset.hpp"
#include "ecs/event/registry.hpp"
#include "ecs/system/group.hpp"

namespace rome::core {
    namespace System {
        using ID = u32;

        struct Context;

        /**
         * @brief A system's descriptor defines its static properties such as name, scheduling, etc.
         */
        struct Descriptor {
            const Component::Registry& components;   ///< Reference to the component registry.
            std::string name;                        ///< The name of the system. Must be unique.
            ID id;                                   ///< The unique identifier of the system.
            std::function<void(Context&)> callback;  ///< The function to be called every time the system is executed.
            BitSet<Component::ID> reads;             ///< The components this system reads.
            BitSet<Component::ID> writes;            ///< The components this system writes.
            BitSet<Event::ID> emits;                 ///< The events this system emits.
            BitSet<Event::ID> listens;               ///< The events this system listens to.
            b8 requireFull;                          ///< Whether the system must operate on a full-owning group.
            b8 allowPartial;                         ///< Whether the system can operate on partial groups.

            Group getGroup() const;
        };

        class Builder {
            public:
            Builder(const std::string& name, Component::Registry& components, Event::Registry& events);

            template <Component::Component... Args>
            Builder& reads() {
                descriptor.reads = std::move(BitSet<>::create({components.enter<Args>()...}));
                return *this;
            }

            template <Component::Component... Args>
            Builder& writes() {
                descriptor.writes = std::move(BitSet<>::create({components.enter<Args>()...}));
                return *this;
            }

            Builder& emits(std::initializer_list<Event::ID> events);

            Builder& listens(std::initializer_list<Event::ID> events);

            Builder& requireFull(b8 value = true);

            Builder& allowPartial(b8 value = true);

            Descriptor build();

            private:
            Descriptor descriptor;            ///< The system's descriptor being built.
            Component::Registry& components;  ///< Reference to the component registry.
            Event::Registry& events;          ///< Reference to the event registry.
        };
    }  // namespace System
}  // namespace rome::core