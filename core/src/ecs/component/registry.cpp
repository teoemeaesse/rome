#include "ecs/component/registry.hpp"

namespace rome::core {
    namespace Component {
        u32 Registry::getCount() const { return store.size(); }

        const std::string& Registry::getName(ID id) const {
            auto it = names.find(id);
            if (it != names.end()) {
                return it->second;
            }
            std::string msg = "Component ID " + std::to_string(id) + " not found";
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
        }
    }  // namespace Component
}  // namespace rome::core