#pragma once

#include <vector>

#include "reflection/reflect.hpp"

namespace iodine::core {
    class Fields;

    /**
     * @brief A reflectable struct or class field.
     */
    class Field {
        friend class Fields;

        public:
        inline const Type& getType() const { return type; }
        inline const char* getName() const { return name; }

        /**
         * @brief Gets the value of the field from the given container.
         * @tparam T The contained type of the field.
         * @param container The container to get the value from.
         * @return The value of the field.
         */
        template <typename T>
        inline T* getValue(void* container) noexcept {
            if (!container || !isType<T>()) {
                return nullptr;
            }
            return static_cast<T*>(reinterpret_cast<byte*>(container) + offset);
        }

        /**
         * @brief Checks if the field is of the given type.
         * @tparam T The contained type to check.
         * @return True if the field is of the given type, false otherwise.
         */
        template <typename T>
        inline b8 isType() noexcept {
            return type.getUUID() == Type::getUUID<T>();
        }

        private:
        const Type& type;  ///< The type of the field.
        const char* name;  ///< The name of the field.
        const u64 offset;  ///< The offset of the field in its containing struct.

        /**
         * @brief Creates a new field with the given type, name, and value.
         * @param type The type of the field.
         * @param name The name of the field.
         * @param offset The offset of the field in its containing struct.
         */
        Field(const Type& type, const char* name, u64 offset);

        /**
         * @brief Creates a new field given a name and a pointer to member.
         * @tparam S The struct/class type that owns the member.
         * @tparam M The type of the member.
         * @param name The name of the field.
         * @param member A pointer to the member (e.g. &S::myMember).
         * @return A new Field instance with the correct offset already computed.
         */
        template <typename S, typename M>
        static Field make(const char* name, M S::* member) {
            // Ensure that the struct is standard layout to use the pointer-to-member offset trick.
            STATIC_ASSERT(std::is_standard_layout_v<S>, "Pointer-to-member offset trick requires standard layout");
            return Field(reflect<M>(), name, reinterpret_cast<u64>(&(reinterpret_cast<S*>(0)->*member)));
        }
    };

    /**
     * @brief A list of reflectable fields. Stores any number of Field<T> objects in a tuple.
     * @tparam FieldList The fields to list.
     */
    class Fields : public Trait {
        public:
        Fields();

        /**
         * @brief Adds a new field to the list.
         * @tparam S The struct/class type that owns the member.
         * @tparam M The type of the member.
         * @param name The name of the field.
         * @param member A pointer to the member (e.g. &S::myMember).
         * @return A reference to this Fields instance.
         */
        template <typename S, typename M>
        Fields& with(const char* name, M S::* member) {
            fields.emplace_back(Field::make(name, member));
            return *this;
        }

        /* Non-const iterator interfaces */
        inline std::vector<Field>::iterator begin() { return fields.begin(); }
        inline std::vector<Field>::iterator end() { return fields.end(); }

        /* Const iterator interfaces */
        inline std::vector<Field>::const_iterator begin() const { return fields.begin(); }
        inline std::vector<Field>::const_iterator end() const { return fields.end(); }

        private:
        std::vector<Field> fields;  ///< The list of fields.
    };
}  // namespace iodine::core