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
    iodine::Unique<iodine::core::Type> type::reflect() { return iodine::core::Type::make(demangle(typeid(type).name()).c_str(), __VA_ARGS__); }

namespace iodine::core {
    /**
     * @brief A generic type that can be reflected.
     */
    class Type {
        public:
        virtual ~Type() = default;

        /**
         * @brief Creates a new type with the given traits.
         * @tparam ...Traits The trait types.
         * @param name The name of the type.
         * @param traits The traits for the type.
         * @return The new type.
         */
        template <typename... Traits>
        static Unique<Type> make(const char* name, const Traits&... traits);

        inline const char* getName() const { return name; }
        virtual inline const UUID getUUID() const = 0;

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
     * @tparam Traits The traits of the type.
     */
    template <typename... Traits>
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
        UUID uuid;                     ///< The UUID of the type.

        /**
         * @brief Creates a new type.
         * @param name The name of the type.
         * @param traits The traits of the type.
         */
        ConcreteType(const char* name, const Traits&... traits) : Type(name), traits(traits...) {
            static const UUID uuid = uuids.generate();
            this->uuid = uuid;
        }
    };

    template <typename... Traits>
    Unique<Type> Type::make(const char* name, const Traits&... traits) {
        STATIC_ASSERT((std::is_base_of<Trait, Traits>::value && ...), "Traits must inherit from Trait");
        ConcreteType<Traits...>* ptr = new ConcreteType<Traits...>(name, traits...);
        return Unique<Type>(ptr);
    }
}  // namespace iodine::core