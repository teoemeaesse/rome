#pragma once

#include "ecs/system/group.hpp"

namespace rome::core {
    namespace System {
        class Registry {
            public:
            Registry();

            void addSystem() {}

            private:
            std::vector<Group> groups;  ///< The groups managed by this registry.
        };
    }  // namespace System
}  // namespace rome::core