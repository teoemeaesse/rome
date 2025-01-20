#pragma once

#include "reflection/reflect.hpp"

namespace iodine::core {
    /**
     * @brief A reflectable struct or class field.
     */
    class Field {
        public:
        /**
         * @brief Creates a new field with the given name and value.
         * @tparam T The type of the field.
         * @param name The name of the field.
         * @param value The value of the field.
         * @return The new field.
         */
        template <typename T>
        static Field make(const char* name, T* value) {
            return Field(Type::getUUID<T>(), name, value);
        }

        inline void* getValue() { return value; }
        inline const char* getName() const { return name; }

        private:
        const UUID type;
        const char* name;
        void* value;

        Field(UUID type, const char* name, void* value) : type(type), name(name), value(value) {}
    };

    /**
     * @brief A list of reflectable fields. Stores any number of Field<T> objects in a tuple.
     * @tparam FieldList The fields to list.
     */
    class Fields : public Trait {
        public:
        template <typename... FieldList>
        Fields(FieldList&&... fields) : Trait("Fields") {
            STATIC_ASSERT((std::is_base_of<Field, FieldList>::value && ...), "FieldList must inherit from Field");
            (this->fields.emplace_back(fields), ...);
        }

        private:
        std::vector<Field> fields;
    };
}  // namespace iodine::core