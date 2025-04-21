#include "reflection/trait.hpp"

namespace iodine::core {
    Trait::Trait(const char* name, const UUID uuid) : name(name), uuid(uuid) {}
}