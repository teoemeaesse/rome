#include "reflection/traits/field.hpp"

namespace iodine::core {
    Field::Field(const Type& type, const char* name, u64 offset) : type(type), name(name), offset(offset) {}

    Fields::Fields() : Trait("Fields", getUUID<Fields>()) {}

    Field* Fields::find(const char* fieldName) {
        auto it = std::find_if(fields.begin(), fields.end(), [fieldName](const Field& f) {
            // Protect against null names or fieldName
            if (!fieldName || !f.getName()) return false;
            return std::strcmp(f.getName(), fieldName) == 0;
        });
        if (it == fields.end()) {
            return nullptr;
        }
        return &(*it);
    }

    const Field* Fields::find(const char* fieldName) const {
        auto it = std::find_if(fields.begin(), fields.end(), [fieldName](const Field& f) {
            if (!fieldName || !f.getName()) return false;
            return std::strcmp(f.getName(), fieldName) == 0;
        });
        if (it == fields.end()) {
            return nullptr;
        }
        return &(*it);
    }
}  // namespace iodine::core