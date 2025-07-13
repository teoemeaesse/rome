#include "ecs/system/registry.hpp"

#include "ecs/system/descriptor.hpp"

namespace rome::core {
    namespace System {
        ID Registry::enter(Descriptor&& descriptor) {
            std::unique_lock lock(systemsLock);
            ID id;
            if (!freeIDs.empty()) {
                id = freeIDs.front();
                freeIDs.pop();
            } else {
                static ID nextId = 0;
                id = nextId++;
            }

            if (ids.find(descriptor.name) != ids.end()) {
                THROW_CORE_EXCEPTION(Exception::Type::InvalidArgument, "Duplicate system name");
            }
            if (descriptors.find(id) != descriptors.end()) {
                THROW_CORE_EXCEPTION(Exception::Type::InvalidArgument, "Duplicate system ID");
            }

            ids.emplace(descriptor.name, id);
            names.emplace(id, descriptor.name);
            descriptors.emplace(id, std::move(descriptor));

            return id;
        }

        b8 Registry::contains(ID id) const noexcept { return descriptors.find(id) != descriptors.end(); }

        Descriptor& Registry::get(ID id) {
            try {
                return descriptors.at(id);
            } catch (const std::out_of_range&) {
                std::string msg = "System with ID " + std::to_string(id) + " not found";
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
            }
        }

        const Descriptor& Registry::get(ID id) const {
            try {
                return descriptors.at(id);
            } catch (const std::out_of_range&) {
                std::string msg = "System with ID " + std::to_string(id) + " not found";
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
            }
        }

        void Registry::erase(ID id) {
            std::unique_lock lock(systemsLock);
            auto it = descriptors.find(id);
            if (it == descriptors.end()) return;
            ids.erase(names[id]);
            names.erase(id);
            descriptors.erase(it);
            freeIDs.push(id);
        }
    }  // namespace System
}  // namespace rome::core