#pragma once

#include "platform/platform.hpp"

namespace iodine::core {
    /**
     * @brief An RFC-9562 compliant UUID (Universally Unique Identifier).
     * @note Supports UUIDv1 and UUIDv5.
     */
    class UUID {
        public:
        UUID() : bytes{0} {}
        /**
         * @brief Create a UUID from a string.
         * @param str The string to create the UUID from.
         * @note The string must be in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" where x is a hexadecimal digit.
         */
        UUID(const char* str);
        UUID(const std::string& str);
        UUID(const UUID&) = default;
        UUID(UUID&&) = default;
        UUID& operator=(const UUID&) = default;
        UUID& operator=(UUID&&) = default;

        inline b8 operator==(const UUID& other) const noexcept { return memcmp(bytes, other.bytes, sizeof(bytes)) == 0; }
        inline b8 operator!=(const UUID& other) const noexcept { return memcmp(bytes, other.bytes, sizeof(bytes)) != 0; }

        /**
         * @brief Convert the UUID to a string.
         * @return The UUID as a string.
         */
        operator std::string() const;

        /**
         * @brief Get the UUID version i.e. UUIDv4 -> 4.
         * @return The UUID as a string.
         */
        u8 getVersion() const;

        private:
        u8 bytes[16];  ///< The bytes of the UUID.
    };
}  // namespace iodine::core