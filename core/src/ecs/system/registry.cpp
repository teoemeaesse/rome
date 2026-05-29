#include "rm/ecs/system/registry.hpp"

#include <string_view>

#include "rm/ecs/system/descriptor.hpp"
#include "rm/ecs/system/view.hpp"

namespace rome::core {
    namespace System {
        b8 Registry::submit(Descriptor&& descriptor) {
            if (descriptor.name.empty() || ids.find(descriptor.name) != ids.end()) {
                return false;
            }

            ID id;
            if (!freeIDs.empty()) {
                id = freeIDs.front();
                freeIDs.pop();
            } else {
                id = nextId++;
            }

            if (descriptors.find(id) != descriptors.end()) {
                return false;
            }

            ids.emplace(descriptor.name, id);
            names.emplace(id, descriptor.name);
            descriptors.emplace(id, std::move(descriptor));
            auto [groupIt, groupInserted] = groups.try_emplace(id, descriptors.at(id));
            RM_ASSERT(groupInserted);

            const Entity::Registry& entities = descriptors.at(id).world.entities;
            for (const Entity entity : entities) {
                if (groupIt->second.matches(entity)) groupIt->second.addEntity(entity);
            }

            return true;
        }

        b8 Registry::revoke(ID id) {
            auto it = descriptors.find(id);
            if (it == descriptors.end()) return false;
            descriptors.erase(it);
            ids.erase(names[id]);
            names.erase(id);
            groups.erase(id);
            freeIDs.push(id);
            return true;
        }

        b8 Registry::check(std::string_view name) const noexcept { return descriptors.find(getID(name)) != descriptors.end(); }

        b8 Registry::check(ID id) const noexcept { return descriptors.find(id) != descriptors.end(); }

        ID Registry::getID(std::string_view name) const noexcept {
            auto it = ids.find(name);
            return it != ids.end() ? it->second : INVALID_ID;
        }

        Descriptor& Registry::get(std::string_view name) { return get(getID(name)); }

        const Descriptor& Registry::get(std::string_view name) const { return get(getID(name)); }

        Descriptor& Registry::get(ID id) {
            auto it = descriptors.find(id);
            if (it != descriptors.end()) return it->second;

            std::string msg = "System with ID " + std::to_string(id) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }

        const Descriptor& Registry::get(ID id) const {
            auto it = descriptors.find(id);
            if (it != descriptors.end()) return it->second;

            std::string msg = "System with ID " + std::to_string(id) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }

        Group& Registry::getGroup(std::string_view name) { return getGroup(getID(name)); }

        const Group& Registry::getGroup(std::string_view name) const { return getGroup(getID(name)); }

        Group& Registry::getGroup(ID id) {
            auto it = groups.find(id);
            if (it != groups.end()) return it->second;

            std::string msg = "Group for system with ID " + std::to_string(id) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }

        const Group& Registry::getGroup(ID id) const {
            auto it = groups.find(id);
            if (it != groups.end()) return it->second;

            std::string msg = "Group for system with ID " + std::to_string(id) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }

        void Registry::updateEntity(Entity entity) {
            for (auto& [id, group] : groups) {
                const b8 matched = group.matches(entity);
                const b8 tracked = group.contains(entity);

                if (matched && !tracked) {
                    group.addEntity(entity);
                } else if (!matched && tracked) {
                    group.removeEntity(entity);
                }
            }
        }

        void Registry::removeEntity(Entity entity) {
            for (auto& [id, group] : groups) {
                group.removeEntity(entity);
            }
        }

        void Registry::run(ID id) {
            Descriptor& descriptor = get(id);
            if (!descriptor.active) return;

            Context context{getGroup(id), descriptor.world};
            descriptor.callback(context);
        }
    }  // namespace System
}  // namespace rome::core
