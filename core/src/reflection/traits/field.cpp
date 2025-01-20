#include "reflection/traits/field.hpp"

namespace iodine::core {
    Field::Field(const Type& type, const char* name, u64 offset) : type(type), name(name), offset(offset) {}

    Fields::Fields() : Trait(Trait::getUUID<Fields>(), "Fields") {}
}  // namespace iodine::core