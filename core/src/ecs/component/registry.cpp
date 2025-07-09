#include "ecs/component/registry.hpp"

namespace iodine::core {
    namespace Component {
        u32 Registry::count() const {
            std::shared_lock readLock(idsLock);
            return store.size();
        }

        const std::string& Registry::name(ID id) const {
            std::shared_lock readLock(idsLock);
            auto it = names.find(id);
            if (it != names.end()) {
                return it->second;
            }
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, (std::string("Component ") + std::to_string(id) + " not found").c_str());
        }
    }  // namespace Component
}  // namespace iodine::core