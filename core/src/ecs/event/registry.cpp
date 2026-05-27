#include "rm/ecs/event/registry.hpp"

#include "rm/ecs/event/event.hpp"

namespace rome::core {
    namespace Event {
        b8 Registry::submit(const std::string_view name) {
            if (name.empty()) return false;

            std::unique_lock lock(eventsLock);
            if (!check(name)) return false;

            if (ids.contains(name)) {
                return false;
            } else {
                ID id;
                if (!freeIDs.empty()) {
                    id = freeIDs.front();
                    freeIDs.pop();
                } else {
                    id = nextId++;
                }

                ids.emplace(std::string(name), id);
                names.emplace(id, name);
                return true;
            }
        }

        b8 Registry::revoke(const std::string_view name) {
            if (name.empty()) return false;

            std::unique_lock lock(eventsLock);
            const ID id = getID(name);
            if (id == INVALID_ID) return false;

            ids.erase(std::string(name));
            names.erase(id);
            freeIDs.push(id);
            return true;
        }

        b8 Registry::check(const std::string_view name) const { return !name.empty() && ids.contains(name); }

        ID Registry::getID(const std::string_view name) const {
            auto it = ids.find(name);
            return it != ids.end() ? it->second : INVALID_ID;
        }
    }  // namespace Event
}  // namespace rome::core
