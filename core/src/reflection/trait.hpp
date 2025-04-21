#pragma once

#include "debug/exception.hpp"
#include "reflection/uuid.hpp"

namespace iodine::core {
    /**
     * @brief A reflectable property of a type.
     */
    class IO_API Trait {
        public:
        virtual ~Trait() = default;

        inline b8 operator==(const Trait& other) const noexcept { return uuid == other.uuid; }
        inline b8 operator!=(const Trait& other) const noexcept { return uuid != other.uuid; }

        /**
         * @brief Gets the UUID for the trait.
         * @tparam T The type of the trait.
         * @return The UUID for the trait.
         */
        template <typename T>
        static inline UUID getUUID() {
            CORE_ASSERT_EXCEPTION((std::is_base_of_v<Trait, T>), "T is not a trait");
            static const UUID id;
            return id;
        }

        /**
         * @brief Checks if the trait is of the given type.
         * @tparam T The type of the trait.
         * @param trait The trait to check.
         * @return True if the trait is of the given type, false otherwise.
         */
        template <typename T>
        static inline b8 is(const Trait& trait) {
            CORE_ASSERT_EXCEPTION((std::is_base_of_v<Trait, T>), "T is not a trait");
            return trait.getUUID() == getUUID<T>();
        }

        inline UUID getUUID() const noexcept { return uuid; }
        inline const char* getName() const noexcept { return name; }

        protected:
        /**
         * @brief Creates a new trait with the given name.
         * @param name The name of the trait.
         * @param uuid The UUID of the trait.
         */
        Trait(const char* name, const UUID uuid);

        private:
        const UUID uuid;   ///< The UUID of the trait.
        const char* name;  ///< The name of the trait. Should be unique for each trait type.
    };
}  // namespace iodine::core