#pragma once

#include "reflection/uuid.hpp"

namespace iodine::core {
    /**
     * @brief A reflectable property of a type.
     */
    class Trait {
        public:
        /**
         * @brief Creates a new trait with the given name.
         * @param name The name of the trait.
         */
        Trait(const char* name);
        virtual ~Trait() = default;

        /**
         * @brief Gets the UUID for the trait.
         * @tparam T The type of the trait.
         * @return The UUID for the trait.
         */
        template <typename T>
        static UUID getUUID() {
            STATIC_ASSERT((std::is_base_of<Trait, T>::value), "T is not a trait");
            static const UUID id = UUIDGenerator().generate();
            return id;
        }

        bool operator==(const Trait& other) const;
        bool operator!=(const Trait& other) const;

        private:
        const char* name;  ///< The name of the trait. Should be unique for each trait type.
    };
}  // namespace iodine::core