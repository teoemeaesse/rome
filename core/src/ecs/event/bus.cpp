#include "ecs/event/bus.hpp"

namespace rome::core {
    namespace Event {
        void Bus::swap() {
            for (auto& [_, q] : queues) {
                q->swap();
            }
        }
    }  // namespace Event
}  // namespace rome::core