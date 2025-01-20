#include "reflection/trait.hpp"

namespace iodine::core {
    Trait::Trait(const char* name) : name(name) {}

    bool Trait::operator==(const Trait& other) const { return name == other.name; }

    bool Trait::operator!=(const Trait& other) const { return name != other.name; }
}