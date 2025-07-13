#pragma once

#include "ecs/component/registry.hpp"
#include "ecs/entity/registry.hpp"
#include "ecs/event/registry.hpp"

namespace rome::core {
    namespace System {
        class Registry;
    }

    struct RM_API World {
        System::Registry& systems;        ///< The registry for all systems in the ECS.
        Component::Registry& components;  ///< The registry for all components in the ECS.
        Entity::Registry& entities;       ///< The registry for all entities in the ECS.
        Event::Registry& events;          ///< The registry for all events in the ECS.
    };
}  // namespace rome::core