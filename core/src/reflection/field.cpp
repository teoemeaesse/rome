#include "reflection/field.hpp"

namespace iodine::core {
    Field::Field(const Type& type, const char* name, u64 offset) : type(type), name(name), offset(offset) {}
}  // namespace iodine::core