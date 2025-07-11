#include "ecs/event/registry.hpp"

#include "debug/exception.hpp"

namespace rome::core {
    namespace Event {
        ID Registry::enter(const std::string& name) {
            {
                std::shared_lock lock(eventsLock);
                if (ids.contains(name)) {
                    return ids[name];
                }
            }

            std::unique_lock lock(eventsLock);
            if (ids.contains(name)) {
                return ids[name];
            } else {
                ID id;
                if (!freeIDs.empty()) {
                    id = freeIDs.front();
                    freeIDs.pop();
                } else {
                    id = static_cast<ID>(ids.size());
                }

                ids[name] = id;
                names[id] = name;
                return id;
            }
        }

        ID Registry::get(const std::string& name) const {
            auto it = ids.find(name);
            if (it != ids.end()) {
                return it->second;
            }
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Event not found");
        }
    }  // namespace Event
}  // namespace rome::core