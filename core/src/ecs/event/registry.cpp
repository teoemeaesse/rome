#include "rm/ecs/event/registry.hpp"

namespace rome::core {
    namespace Event {
        ID Registry::enter(const std::string& name) {
            if (name.empty()) return INVALID_ID;

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
                    id = nextId++;
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
            return INVALID_ID;
        }

        b8 Registry::contains(const std::string& name) const { return ids.contains(name); }
    }  // namespace Event
}  // namespace rome::core
