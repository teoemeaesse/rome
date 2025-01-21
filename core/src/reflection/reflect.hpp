#pragma once

#include <vector>

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
    inline iodine::core::Type& iodine::core::Reflect::reflect<type>() {                                      \
        static iodine::core::Type instance = iodine::core::Type::make<type>(name __VA_OPT__(, __VA_ARGS__)); \
        return instance;                                                                                     \
    }

namespace iodine::core {
    /**
     * @brief A generic type that can be reflected.
     */
    class Type {
        public:
        virtual ~Type() = default;
        Type(const Type&) = default;
        Type(Type&&) = default;

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
        static Type make(const std::string& name, Traits&&... traits) {
            STATIC_ASSERT((std::is_base_of_v<Trait, std::remove_reference_t<Traits>> && ...), "Traits must inherit from Trait");
            return Type(Type::getUUID<T>(), name, std::forward<Traits>(traits)...);
        }

        inline const std::string& getName() const noexcept { return name; }
        inline const UUID getUUID() const noexcept { return uuid; }

        /**
         * @brief Statically queries the UUID for the given type.
         * @tparam T The type to get the UUID for.
         * @return The UUID for the type.
         */
        template <typename T>
        static const UUID getUUID() noexcept {
            // This is templated to ensure that the UUID is unique for each type, primitive or not.
            static const UUID uuid = uuids.generate();
            return uuid;
        }

        /**
         * @brief Checks if the type has the given trait.
         * @tparam T The trait type.
         * @return True if the type has the trait, false otherwise.
         */
        template <typename T>
        inline b8 hasTrait() const noexcept {
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
        inline const T& getTrait() const {
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
        inline T& getTrait() {
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
    };

    /**
     * @brief Declares a generic reflection for a given type.
     * @note Mostly just here to circumvent private member access.
     */
    struct Reflect {
        template <typename T>
        inline static Type& reflect();
    };
}  // namespace iodine::core