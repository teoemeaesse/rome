#include "ecs/system/group.hpp"

namespace iodine::core {
    namespace System {
        Group::Group(BitSet<Component::ID> owning, BitSet<Component::ID> partial, const Component::Registry& components)
            : owning(std::move(owning)), partial(std::move(partial)), components(components) {}

        const std::string& Group::toString() const {
            static std::string cached = [this] {
                std::string s;
                auto emit = [&](Component::ID id, char prefix) {
                    if (!s.empty()) s += ", ";
                    s += prefix;
                    s += components.name(id);
                };

                for (Component::ID id = 0; id < components.count(); id++) {
                    if (owning.test(id))
                        emit(id, '+');
                    else if (partial.test(id))
                        emit(id, '~');
                }
                return s;
            }();
            return cached;
        }
        // Additional methods can be added here if needed
    }  // namespace System
}  // namespace iodine::core