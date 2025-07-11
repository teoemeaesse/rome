#include "reflection/trait.hpp"

namespace rome::core {
    Trait::Trait(const char* name, const UUID uuid) : name(name), uuid(uuid) {}
}