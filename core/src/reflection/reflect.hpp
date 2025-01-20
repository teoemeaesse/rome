#pragma once

#include <vector>

#include "reflection/trait.hpp"

/**
 * @brief Implements reflection for the containing type. You should create a type with your own traits here.
 * @param type The name of the type to reflect.
 * @param ... The traits of the type.
 */
#define IO_REFLECT(type, ...)                                                             \
    template <>                                                                           \
    inline const iodine::core::Type& iodine::core::reflect<type>() {                      \
        static iodine::core::Type instance = iodine::core::Type::make<type>(__VA_ARGS__); \
        return instance;                                                                  \
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
         * @param traits The traits for the type.
         * @return The new type.
         */
        template <typename T, typename... Traits>
        static Type make(Traits&&... traits) {
            STATIC_ASSERT((std::is_base_of_v<Trait, std::remove_reference_t<Traits>> && ...), "Traits must inherit from Trait");
            return Type(Type::getUUID<T>(), demangle(typeid(T).name()), std::forward<Traits>(traits)...);
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

        protected:
        /**
         * @brief Creates a new type with the given name and UUID.
         * @param uuid The UUID of the type.
         * @param name The name of the type.
         * @param traits The traits of the type.
         */
        template <typename... Traits>
        Type(UUID uuid, const std::string& name, Traits&&... traits) : uuid(uuid), name(name) {
            (this->traits.push_back(MakeUnique<std::remove_reference_t<Traits>>(std::forward<Traits>(traits))), ...);
        }

        private:
        const UUID uuid;                    ///< The UUID of the type.
        const std::string name;             ///< The name of the type.
        std::vector<Unique<Trait>> traits;  ///< The traits of the type.
    };

    template <typename T>
    inline const Type& reflect();
}  // namespace iodine::core