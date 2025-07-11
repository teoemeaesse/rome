#pragma once

#include "platform/platform.hpp"

namespace rome::core {
    /**
     * @brief An RFC 1321 compliant MD5 hash implementation.
     * @details This class provides a simple interface to create MD5 hashes from strings or byte arrays.
     *          It is not thread-safe and should be used in a single-threaded context.
     */
    class MD5 {
        public:
        /**
         * @brief Create an MD5 hash from a string.
         * @param input The string to hash.
         */
        MD5(const std::string& input);
        /**
         * @brief Create an MD5 hash from a byte array.
         * @param input The byte array to hash.
         * @param length The length of the byte array.
         */
        MD5(const char* input, u32 length);
        MD5(const MD5&) = default;
        MD5(MD5&&) = default;
        MD5& operator=(const MD5&) = default;
        MD5& operator=(MD5&&) = default;
        ~MD5() = default;

        inline b8 operator==(const MD5& other) const noexcept { return memcmp(digest, other.digest, sizeof(digest)) == 0; }
        inline b8 operator!=(const MD5& other) const noexcept { return memcmp(digest, other.digest, sizeof(digest)) != 0; }

        /**
         * @brief Get the MD5 hash as a string.
         * @return The MD5 hash as a string.
         */
        operator std::string() const;

        struct Context;

        private:
        u8 digest[16];  ///< Hash.

        /**
         * @brief Main hashing function.
         * @param input The input string.
         * @param length The length of the input string.
         */
        void hash(const char* input, u32 length);
    };
}  // namespace rome::core