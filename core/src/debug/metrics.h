#pragma once

#include <mutex>
#include <unordered_map>

#include "debug/log.h"
#include "prelude.h"

namespace iodine::core {
    /**
     * @brief Tracks program time and memory performance over time.
     */
    class Metrics {
        public:
        Metrics() = default;
        ~Metrics() = default;
        Metrics(const Metrics&) = delete;
        Metrics(Metrics&&) = delete;
        Metrics& operator=(const Metrics&) = delete;
        Metrics& operator=(Metrics&&) = delete;

        /**
         * @brief Gets the singleton instance of the metrics.
         * @return The metrics instance.
         */
        static Metrics& getInstance() {
            static Metrics instance;
            return instance;
        }

        /**
         * @brief Registers a heap memory allocation.
         * @param ptr The pointer returned by the underlying allocation.
         * @param size The number of bytes requested.
         */
        void registerAllocation(void* ptr, u64 size);

        /**
         * @brief Registers a heap memory free operation.
         * @param ptr The pointer being freed.
         */
        void registerDeallocation(void* ptr);

        /**
         * @brief Gets a string representation of the memory metrics.
         * @return The memory metrics as a string.
         */
        std::string getMemoryMetrics() const;

        /**
         * @brief Gets the total number of bytes currently heap-allocated.
         * @return The number of bytes currently allocated.
         */
        inline u64 getCurrentBytes() const { return currentBytes; }

        /**
         * @brief Gets the peak number of bytes that have been allocated during program execution.
         * @return The peak number of bytes allocated.
         */
        inline u64 getPeakBytes() const { return peakBytes; }

        /**
         * @brief Gets the total number of bytes allocated during program execution.
         * @return The total number of bytes allocated.
         */
        inline u64 getTotalBytes() const { return totalBytes; }

        /**
         * @brief Gets the total number of heap allocations during program execution.
         * @return The total number of heap allocations.
         */
        inline u64 getTotalAllocations() const { return totalAllocations; }

        /**
         * @brief Gets the number of heap allocations that have not been deallocated.
         * @return The number of missing deallocations.
         */
        inline u64 getMissingDeallocations() const { return allocations.size(); }

        /**
         * @brief Disables memory logging.
         */
        inline void disableMemory() { memoryLogging = false; }

        /**
         * @brief Enables memory logging.
         */
        inline void enableMemory() { memoryLogging = true; }

        private:
        bool memoryLogging = false;                  ///< Whether to log memory allocation and deallocation.
        std::unordered_map<void*, u64> allocations;  ///< Tracks each pointer's allocated size.
        mutable std::mutex registrarMutex;           ///< Protects allocations and the counters from concurrent access.
        u64 currentBytes = 0;                        ///< The current total heap-allocated bytes.
        u64 peakBytes = 0;                           ///< The maximum number of bytes allocated during program execution.
        u64 totalBytes = 0;                          ///< The total number of bytes allocated during program execution.
        u64 totalAllocations = 0;                    ///< The total number of heap allocations.
    };
}  // namespace iodine::core