#pragma once

#include <atomic>

#include "platform/platform.hpp"

namespace iodine::core {
    /**
     * @brief A universally unique identifier. Only unique to each instance of its generator.
     */
    using UUID = u64;

    /**
     * @brief Class for generating universally unique identifiers.
     * @note UUID's are only unique for their generator. Do not share them between different applications.
     */
    class IO_API UUIDGenerator {
        public:
        UUIDGenerator();
        UUIDGenerator(const UUIDGenerator&) = delete;
        UUIDGenerator(UUIDGenerator&&) = delete;
        UUIDGenerator& operator=(const UUIDGenerator&) = delete;
        UUIDGenerator& operator=(UUIDGenerator&&) = delete;

        /**
         * @brief Generates a new universally unique identifier.
         * @return The new universally unique identifier.
         * @note Thread-safe.
         */
        UUID generate();

        private:
        u64 key;                   ///< The random key used for masking.
        std::atomic<u64> counter;  ///< Monotonically increasing counter.
    };

    static UUIDGenerator uuids;  ///< Generates internal UUIDs. Do not use for persistent storage of UUIDs.
}  // namespace iodine::core