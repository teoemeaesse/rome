#pragma once

#include <vector>

#include "reflection/trait.hpp"

/**a
 * @brief Declares reflection for the containing type.
 */
#define IO_REFLECT() iodine::Unique<iodine::core::Type> reflect();

/**
 * @brief Implements reflection for the containing type. You should create a type with your own traits here.
 * @param type The name of the type to reflect.
 * @param ... The traits of the type.
 */
#define IO_REFLECT_IMPL(type, ...) \
    iodine::Unique<iodine::core::Type> type::reflect() { return iodine::core::Type::make<type>(__VA_ARGS__); }

namespace iodine::core {
    /**
     * @brief A generic type that can be reflected.
     */
    class Type {
        public:
        virtual ~Type() = default;

        /**
         * @brief Creates a new type with the given traits.
         * @tparam T The underlying type.
         * @tparam ...Traits The trait types.
         * @param traits The traits for the type.
         * @return The new type.
         */
        template <typename T, typename... Traits>
        static Unique<Type> make(const Traits&... traits);

        inline const char* getName() const { return name; }
        virtual inline const UUID getUUID() const = 0;

        /**
         * @brief Statically queries the UUID for the given type.
         * @tparam T The type to get the UUID for.
         * @return The UUID for the type.
         */
        template <typename T>
        static const UUID getUUID() {
            // This is templated to ensure that the UUID is unique for each type, primitive or not.
            static const UUID uuid = uuids.generate();
            return uuid;
        }

        protected:
        /**
         * @brief Creates a new type with the given name.
         * @param name The name of the type.
         */
        Type(const char* name);

        private:
        const char* name;  ///< The name of the type.
    };

    /**
     * @brief A concrete type.
     * @tparam T The underlying type.
     * @tparam Traits The traits of the type.
     */
    template <typename T, typename... Traits>
    class ConcreteType : public Type {
        STATIC_ASSERT((std::is_base_of<Trait, Traits>::value && ...), "Traits must inherit from Trait");

        friend class Type;

        public:
        ~ConcreteType() = default;

        b8 operator==(const ConcreteType& other) const { return uuid == other.uuid; }
        b8 operator!=(const ConcreteType& other) const { return !(uuid == other.uuid); }

        inline const UUID getUUID() const override { return uuid; }

        private:
        std::tuple<Traits...> traits;  ///< The traits of the type.
        const UUID uuid;               ///< The UUID of the type.

        /**
         * @brief Creates a new type.
         * @param traits The traits of the type.
         */
        ConcreteType(const Traits&... traits) : Type(demangle(typeid(T).name()).c_str()), traits(traits...), uuid(Type::getUUID<T>()) {}
    };

    template <typename T, typename... Traits>
    Unique<Type> Type::make(const Traits&... traits) {
        STATIC_ASSERT((std::is_base_of<Trait, Traits>::value && ...), "Traits must inherit from Trait");
        ConcreteType<T, Traits...>* ptr = new ConcreteType<T, Traits...>(traits...);
        return Unique<Type>(ptr);
    }
}  // namespace iodine::core