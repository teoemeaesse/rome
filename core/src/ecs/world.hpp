#pragma once

#include "ecs/component/component.hpp"

namespace iodine::core {
    /**
     * @brief A world is a container for all archetypes, components, resources and systems and serves as the primary interface for the engine's ECS.
     */
    class World {
        public:
        World() = default;
        ~World() = default;

        private:
    };
}  // namespace iodine::core