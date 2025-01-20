#pragma once

#include <tuple>

#include "reflection/trait.hpp"

namespace iodine::core {
    /**
     * @brief A reflectable field.
     * @tparam T The type of the field.
     */
    template <typename T>
    class Field {
        public:
        Field(const char* name, T* value) : value(value), name(name) {}

        inline T* getValue() { return value; }
        inline const char* getName() const { return name; }

        private:
        T* value;
        const char* name;
    };

    /**
     * @brief A list of reflectable fields.
     * @tparam FieldList The fields to list.
     */
    class Fields : public Trait {
        public:
        template <typename... FieldList>
        Fields(FieldList... fields) : Trait("Fields") {}

        private:
    };
}  // namespace iodine::core