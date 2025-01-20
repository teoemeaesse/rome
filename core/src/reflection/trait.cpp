#include "reflection/trait.hpp"

namespace iodine::core {
    Trait::Trait(UUID uuid, const char* name) : uuid(uuid), name(name) {}

    b8 Trait::operator==(const Trait& other) const noexcept { return uuid == other.uuid; }

    b8 Trait::operator!=(const Trait& other) const noexcept { return uuid != other.uuid; }
}