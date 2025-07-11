#pragma once

#include "container/bitset.hpp"
#include "ecs/component/registry.hpp"

namespace rome::core {
    namespace System {
        class RM_API Group final {
            public:
            Group(BitSet<Component::ID> owning, BitSet<Component::ID> partial, const Component::Registry& components);
            ~Group() = default;

            operator const std::string&() const { return toString(); }

            /**
             * @brief Converts the group to a string representation.
             * @return A string representation of the group.
             */
            const std::string& toString() const;

            private:
            const BitSet<Component::ID> owning;     ///< The components that this group fully owns.
            const BitSet<Component::ID> partial;    ///< The components that this group partially owns.
            const Component::Registry& components;  ///< The component registry for accessing component IDs.
        };
    }  // namespace System
}  // namespace rome::core