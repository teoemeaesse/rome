#pragma once

#include "platform/platform.h"

namespace iodine::core {
    /**
     * @brief A universally unique identifier. Only unique to each instance of its generator.
     */
    using UUID = u64;

    /**
     * @brief Class for generating universally unique identifiers.
     * @note UUID's are only unique for their generator. Do not share them between different applications.
     */
    class UUIDGenerator {
        public:
        UUIDGenerator();
        UUIDGenerator(const UUIDGenerator&) = delete;
        UUIDGenerator(UUIDGenerator&&) = default;
        UUIDGenerator& operator=(const UUIDGenerator&) = delete;
        UUIDGenerator& operator=(UUIDGenerator&&) = default;

        /**
         * @brief Generates a new universally unique identifier.
         * @return The new universally unique identifier.
         * @note Thread-safe.
         */
        UUID generate();

        private:
        u64 key;      ///< The random key used for masking.
        u64 counter;  ///< Monotonically increasing counter.
    };
}  // namespace iodine::core