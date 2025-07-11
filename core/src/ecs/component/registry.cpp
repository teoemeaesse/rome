#include "ecs/component/registry.hpp"

namespace rome::core {
    namespace Component {
        u32 Registry::count() const { return store.size(); }

        const std::string& Registry::name(ID id) const {
            auto it = names.find(id);
            if (it != names.end()) {
                return it->second;
            }
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, (std::string("Component ") + std::to_string(id) + " not found").c_str());
        }
    }  // namespace Component
}  // namespace rome::core