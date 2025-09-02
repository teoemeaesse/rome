#include "ecs/event/bus.hpp"

namespace rome::core {
    namespace Event {
        Bus::Bus(World& world) : world(world) {}

        void Bus::swap() {
            for (auto& [_, q] : queues) {
                q->swap();
            }
        }
    }  // namespace Event
}  // namespace rome::core