#include "reflection/traits/field.hpp"

namespace iodine::core {
    Field::Field(const Type& type, const char* name, u64 offset) : type(type), name(name), offset(offset) {}

    Fields::Fields() : Trait("Fields") {}
}  // namespace iodine::core