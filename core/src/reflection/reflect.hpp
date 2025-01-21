#pragma once

#include "reflection/trait.hpp"

/**
 * @brief Declares that a type can be reflected. Slap this on a class, struct or enum to enable reflection.
 */
#define IO_REFLECT friend class iodine::core::Reflect;

/**
 * @brief Implements reflection for the containing type. You list your type's traits here.
 * @param type The name of the type to reflect.
 * @param name The name of the type. This should be unique.
 * @param ... The traits of the type.
 */
#define IO_REFLECT_IMPL(type, name, ...)                                                                     \
    template <>                                                                                              \
    inline iodine::core::Type& iodine::core::Reflect::_reflect<type>() {                                     \
        static iodine::core::Type instance = iodine::core::Type::make<type>(name __VA_OPT__(, __VA_ARGS__)); \
        return instance;                                                                                     \
    }
namespace iodine::core {
    /**
     * @brief A generic type that can be reflected.
     */
    class IO_API Type {
        public:
        ~Type() = default;
        Type(const Type&) = default;
        Type(Type&&) noexcept = default;

        inline b8 operator==(const Type& other) const noexcept { return uuid == other.uuid; }
        inline b8 operator!=(const Type& other) const noexcept { return uuid != other.uuid; }

        /**
         * @brief Creates a new type with the given traits.
         * @tparam T The underlying type.
         * @tparam ...Traits The trait types.
         * @param name The name of the type.
         * @param traits The traits for the type.
         * @return The new type.
         */
        template <typename T, typename... Traits>
        static inline Type make(const std::string& name, Traits&&... traits) {
            STATIC_ASSERT((std::is_base_of_v<Trait, std::remove_reference_t<Traits>> && ...), "Traits must inherit from Trait");
            return Type(Type::getUUID<T>(), name, std::forward<Traits>(traits)...);
        }

        inline const std::string& getName() const noexcept { return name; }
        inline const UUID getUUID() const noexcept { return uuid; }

        /**
         * @brief Statically queries the UUID for a fully qualified type.
         * @tparam T The fully qualified type to get the UUID for.
         * @return The UUID for the type.
         */
        template <typename T>
        static inline const UUID getUUID() noexcept {
            // Remove all qualifiers from the type and then get its UUID - UUID's are unique for each *unqualified* type
            return _getUUID<remove_all_qualifiers_t<T>>();
        }

        /**
         * @brief Checks if the type has the given trait.
         * @tparam T The trait type.
         * @return True if the type has the trait, false otherwise.
         */
        template <typename T>
        b8 hasTrait() const noexcept {
            STATIC_ASSERT((std::is_base_of_v<Trait, T>), "T is not a trait");
            for (const auto& trait : traits) {
                if (Trait::is<T>(*trait)) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Gets the trait of the given type.
         * @tparam T The type of the trait.
         * @return The trait of the given type.
         */
        template <typename T>
        const T& getTrait() const {
            STATIC_ASSERT((std::is_base_of_v<Trait, T>), "T is not a trait");
            for (const auto& trait : traits) {
                if (Trait::is<T>(*trait)) {
                    return static_cast<const T&>(*trait);
                }
            }
            throw std::runtime_error("Type does not have trait");  // TODO: Reflection exceptions
        }

        /**
         * @brief Gets the trait of the given type.
         * @tparam T The type of the trait.
         * @return The trait of the given type.
         */
        template <typename T>
        T& getTrait() {
            STATIC_ASSERT((std::is_base_of_v<Trait, T>), "T is not a trait");
            for (auto& trait : traits) {
                if (Trait::is<T>(*trait)) {
                    return static_cast<T&>(*trait);
                }
            }
            throw std::runtime_error("Type does not have trait");
        }

        protected:
        /**
         * @brief Creates a new type with the given name and UUID.
         * @param uuid The UUID of the type.
         * @param name The name of the type.
         * @param traits The traits of the type.
         */
        template <typename... Traits>
        Type(UUID uuid, const std::string& name, Traits&&... traits) : uuid(uuid), name(name) {
            STATIC_ASSERT((std::is_base_of_v<Trait, std::remove_reference_t<Traits>> && ...), "Traits must inherit from Trait");
            (this->traits.push_back(MakeUnique<std::remove_reference_t<Traits>>(std::forward<Traits>(traits))), ...);
        }

        private:
        const UUID uuid;                    ///< The UUID of the type.
        const std::string name;             ///< The name of the type.
        std::vector<Unique<Trait>> traits;  ///< The traits of the type.

        /**
         * @brief Statically queries the UUID for the given base type.
         * @tparam T The base type to get the UUID for.
         * @return The UUID for the type.
         */
        template <typename T>
        static inline const UUID _getUUID() noexcept {
            // This is templated to ensure that the UUID is unique for each type
            static const UUID uuid = uuids.generate();
            return uuid;
        }
    };

    /**
     * @brief Information about a reflected type such as its base type, size, qualifiers, etc.
     */
    class IO_API TypeInfo {
        public:
        ~TypeInfo() = default;

        inline operator Type&() noexcept { return type; }
        inline operator const Type&() const noexcept { return type; }

        inline Type& getType() noexcept { return type; }
        inline u64 getSize() const noexcept { return size; }
        inline b8 isConstType() const noexcept { return isConst; }
        inline b8 isVolatileType() const noexcept { return isVolatile; }
        inline b8 isPointerType() const noexcept { return isPointer; }
        inline b8 isReferenceType() const noexcept { return isReference; }
        inline b8 isRValueType() const noexcept { return isRValue; }

        /**
         * @brief Reflects the given type, storing some of its metadata.
         * @tparam T The fully-qualified type to reflect.
         * @return The reflection information for the type.
         */
        template <typename T>
        static TypeInfo make(Type& type) {
            return TypeInfo(type, sizeof(remove_all_qualifiers_t<T>), std::is_const_v<T>, std::is_volatile_v<T>, std::is_pointer_v<T>,
                            std::is_reference_v<T>, std::is_rvalue_reference_v<T>);
        }

        private:
        Type& type;            ///< The type that this info is for.
        const u64 size;        ///< The size of the type in bytes.
        const b8 isConst;      ///< True if the type is const, false otherwise.
        const b8 isVolatile;   ///< True if the type is volatile, false otherwise.
        const b8 isPointer;    ///< True if the type is a pointer, false otherwise.
        const b8 isReference;  ///< True if the type is a reference, false otherwise.
        const b8 isRValue;     ///< True if the type is an rvalue reference, false otherwise.

        /**
         * @brief Creates a new type info for the given type.
         * @param type The base type to reflect.
         * @param size The size of the type in bytes.
         * @param isConst True if the type is const, false otherwise.
         * @param isVolatile True if the type is volatile, false otherwise.
         * @param isPointer True if the type is a pointer, false otherwise.
         * @param isRef True if the type is a reference, false otherwise.
         * @param isRValue True if the type is an rvalue reference, false otherwise.
         */
        TypeInfo(Type& type, u64 size, bool isConst, bool isVolatile, bool isPointer, bool isRef, bool isRValue)
            : type(type), size(size), isConst(isConst), isVolatile(isVolatile), isPointer(isPointer), isReference(isRef), isRValue(isRValue) {}
    };

    /**
     * @brief Declares a generic reflection for a given type.
     * @note Mostly just here to circumvent private member access.
     */
    struct IO_API Reflect {
        /**
         * @brief Reflects the given type, storing some of its metadata.
         * @tparam T The fully-qualified type to reflect.
         * @return The reflection information and metadata for the type.
         */
        template <typename T>
        static TypeInfo& reflect() {
            static TypeInfo info = TypeInfo::make<T>(_reflect<remove_all_qualifiers_t<T>>());
            return info;
        }

        private:
        /**
         * @brief Reflect a type. Returns the base type's reflection information.
         * @tparam T The type to reflect.
         * @return The reflection information for the type.
         */
        template <typename T>
        static inline Type& _reflect();
    };
}  // namespace iodine::core