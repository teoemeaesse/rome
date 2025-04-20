#pragma once

#include "prelude.hpp"

namespace iodine::core {
    /**
     * @brief An RFC-9562 compliant UUID (Universally Unique Identifier).
     * @note Supports UUIDv1 and UUIDv5.
     */
    class UUID {
        public:
        /**
         * @brief Creates a UUIDv1.
         */
        UUID();
        UUID(const UUID&) = default;
        UUID(UUID&&) = default;
        UUID& operator=(const UUID&) = default;
        UUID& operator=(UUID&&) = default;

        inline b8 operator==(const UUID& other) const noexcept { return memcmp(bytes, other.bytes, sizeof(bytes)) == 0; }
        inline b8 operator!=(const UUID& other) const noexcept { return memcmp(bytes, other.bytes, sizeof(bytes)) != 0; }

        /**
         * @brief Converts the UUID to a string.
         * @return The UUID as a string.
         */
        operator std::string() const;
        /**
         * @brief Converts the UUID to a string.
         * @return The UUID as a string.
         */
        std::string toString() const;

        /**
         * @brief Gets the UUID version i.e. UUIDv4 -> 4.
         * @return The UUID as a string.
         */
        u8 getVersion() const;

        private:
        u8 bytes[16];  ///< The bytes of the UUID.
    };
}  // namespace iodine::core