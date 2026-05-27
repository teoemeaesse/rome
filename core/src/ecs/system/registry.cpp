#include "rm/ecs/system/registry.hpp"

#include "rm/ecs/system/descriptor.hpp"
#include "rm/ecs/system/view.hpp"

namespace rome::core {
    namespace System {
        b8 Registry::submit(Descriptor&& descriptor) {
            std::unique_lock lock(systemsLock);

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

        b8 Registry::check(ID id) const noexcept {
            std::shared_lock lock(systemsLock);
            return descriptors.find(id) != descriptors.end();
        }

        ID Registry::get(const std::string& name) const noexcept {
            std::shared_lock lock(systemsLock);
            auto it = ids.find(name);
            return it != ids.end() ? it->second : INVALID_ID;
        }

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

        void Registry::updateEntity(const Entity& entity) {
            std::unique_lock lock(systemsLock);

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

        void Registry::removeEntity(const Entity& entity) {
            std::unique_lock lock(systemsLock);

            for (auto& [id, group] : groups) {
                if (group.contains(entity)) group.removeEntity(entity);
            }
        }

        void Registry::run(ID id) {
            Descriptor& descriptor = get(id);
            if (!descriptor.active) return;

            Context context{getGroup(id), descriptor.world};
            descriptor.callback(context);
        }

        void Registry::run() {
            for (auto& [id, descriptor] : descriptors) {
                if (!descriptor.active) continue;

                Context context{getGroup(id), descriptor.world};
                descriptor.callback(context);
            }
        }

        b8 Registry::revoke(ID id) {
            std::unique_lock lock(systemsLock);
            auto it = descriptors.find(id);
            if (it == descriptors.end()) return false;
            ids.erase(names[id]);
            names.erase(id);
            groups.erase(id);
            descriptors.erase(it);
            freeIDs.push(id);
            return true;
        }
    }  // namespace System
}  // namespace rome::core
