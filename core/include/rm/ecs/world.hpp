#pragma once

#include "rm/ecs/component/registry.hpp"
#include "rm/ecs/entity/registry.hpp"
#include "rm/ecs/event/registry.hpp"

namespace rome::core {
    namespace System {
        class Registry;
    }

    struct World {
        System::Registry& systems;        ///< The registry for all systems in the ECS.
        Component::Registry& components;  ///< The registry for all components in the ECS.
        Entity::Registry& entities;       ///< The registry for all entities in the ECS.
        Event::Registry& events;          ///< The registry for all events in the ECS.
    };
}  // namespace rome::core
