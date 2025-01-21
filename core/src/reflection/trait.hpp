#pragma once

#include "reflection/uuid.hpp"

namespace iodine::core {
    /**
     * @brief A reflectable property of a type.
     */
    class IO_API Trait {
        public:
        /**
         * @brief Creates a new trait with the given name.
         * @param uuid The UUID of the trait.
         * @param name The name of the trait.
         */
        Trait(UUID uuid, const char* name);
        virtual ~Trait() = default;

        b8 operator==(const Trait& other) const noexcept;
        b8 operator!=(const Trait& other) const noexcept;

        /**
         * @brief Gets the UUID for the trait.
         * @tparam T The type of the trait.
         * @return The UUID for the trait.
         */
        template <typename T>
        static inline UUID getUUID() noexcept {
            STATIC_ASSERT((std::is_base_of_v<Trait, T>), "T is not a trait");
            static const UUID id = UUIDGenerator().generate();
            return id;
        }

        /**
         * @brief Checks if the trait is of the given type.
         * @tparam T The type of the trait.
         * @param trait The trait to check.
         * @return True if the trait is of the given type, false otherwise.
         */
        template <typename T>
        static inline b8 is(const Trait& trait) noexcept {
            STATIC_ASSERT((std::is_base_of_v<Trait, T>), "T is not a trait");
            return trait.getUUID() == getUUID<T>();
        }

        inline UUID getUUID() const noexcept { return uuid; }
        inline const char* getName() const noexcept { return name; }

        private:
        const UUID uuid;   ///< The UUID of the trait.
        const char* name;  ///< The name of the trait. Should be unique for each trait type.
    };
}  // namespace iodine::core