#pragma once

#include "debug/log.hpp"
#include "reflection/uuid.hpp"

namespace rome::core {
    /**
     * @brief Tracks program time and memory performance over time.
     */
    class RM_API Metrics {
        public:
        Metrics() = default;
        ~Metrics();
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
         * @brief Starts the metrics data gathering.
         */
        void start();

        /**
         * @brief Stops the metrics data gathering.
         */
        void stop();

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
         * @brief Logs the current metrics for all threads.
         */
        void report() const;

        /**
         * @brief Gets a string representation of the memory metrics for the given thread.
         * @param thread The thread to get the memory metrics for.
         * @return The memory metrics as a string.
         */
        std::string getMemoryMetrics(const UUID& thread) const;
        /**
         * @brief Gets a string representation of the memory metrics for the current thread.
         * @return The memory metrics as a string.
         */
        std::string getMemoryMetrics() const;
        /**
         * @brief Gets a string representation of the memory metrics for every tracked thread.
         * @return The memory metrics as a string.
         */
        std::string getGlobalMemoryMetrics() const;

        /**
         * @brief Gets the alias for the given thread.
         * @param thread The thread to get the alias for.
         * @return The thread alias.
         */
        const std::string& getThreadAlias(const UUID& thread) const;
        /**
         * @brief Gets the alias for the current thread.
         * @return The thread alias.
         */
        const std::string& getThreadAlias() const;

        /**
         * @brief Gets the total number of bytes currently heap-allocated by the given thread.
         * @param thread The thread to get the memory metrics for.
         * @return The number of bytes currently allocated.
         */
        u64 getCurrentBytes(const UUID& thread) const;
        /**
         * @brief Gets the total number of bytes currently heap-allocated by the current thread.
         * @return The number of bytes currently allocated.
         */
        u64 getCurrentBytes() const;
        /**
         * @brief Gets the total number of bytes currently heap-allocated by every tracked thread.
         * @return The number of bytes currently allocated.
         */
        u64 getGlobalCurrentBytes() const;

        /**
         * @brief Gets the peak number of bytes that have been allocated during program execution by the given thread.
         * @param thread The thread to get the memory metrics for.
         * @return The peak number of bytes allocated.
         */
        u64 getPeakBytes(const UUID& thread) const;
        /**
         * @brief Gets the peak number of bytes that have been allocated during program execution by the current thread.
         * @return The peak number of bytes allocated.
         */
        u64 getPeakBytes() const;
        /**
         * @brief Gets the peak number of bytes that have been allocated during program execution by every tracked thread.
         * @return The peak number of bytes allocated.
         */
        u64 getGlobalPeakBytes() const;

        /**
         * @brief Gets the total number of bytes allocated during program execution by the given thread.
         * @param thread The thread to get the memory metrics for.
         * @return The total number of bytes allocated.
         */
        u64 getTotalBytes(const UUID& thread) const;
        /**
         * @brief Gets the total number of bytes allocated during program execution by the current thread.
         * @return The total number of bytes allocated.
         */
        u64 getTotalBytes() const;
        /**
         * @brief Gets the total number of bytes allocated during program execution by every tracked thread.
         * @return The total number of bytes allocated.
         */
        u64 getGlobalTotalBytes() const;

        /**
         * @brief Gets the total number of heap allocations during program execution by the given thread.
         * @param thread The thread to get the memory metrics for.
         * @return The total number of heap allocations.
         */
        u64 getTotalAllocations(const UUID& thread) const;
        /**
         * @brief Gets the total number of heap allocations during program execution by the current thread.
         * @return The total number of heap allocations.
         */
        u64 getTotalAllocations() const;
        /**
         * @brief Gets the total number of heap allocations during program execution by every tracked thread.
         * @return The total number of heap allocations.
         */
        u64 getGlobalTotalAllocations() const;

        /**
         * @brief Gets the number of heap allocations that have not been deallocated by the given thread.
         * @param thread The thread to get the memory metrics for.
         * @return The number of missing deallocations.
         */
        u64 getMissingDeallocations(const UUID& thread) const;
        /**
         * @brief Gets the number of heap allocations that have not been deallocated by the current thread.
         * @return The number of missing deallocations.
         */
        u64 getMissingDeallocations() const;
        /**
         * @brief Gets the number of heap allocations that have not been deallocated by every tracked thread.
         * @return The number of missing deallocations.
         */
        u64 getGlobalMissingDeallocations() const;

        /**
         * @brief Gets whether memory tracking is enabled for the given thread.
         * @param thread The thread to check.
         * @return Whether memory tracking is enabled.
         */
        b8 isMemoryTracking(const UUID& thread) const;
        /**
         * @brief Gets whether memory tracking is enabled for the current thread.
         * @return Whether memory tracking is enabled.
         */
        b8 isMemoryTracking() const;

        /**
         * @brief Enables / disables memory tracking for the given thread.
         * @param thread The thread to enable / disable memory tracking for.
         */
        void setIsMemoryTracking(const UUID& thread, b8 isTracking) const;
        /**
         * @brief Enables / disables memory tracking for the current thread.
         * @param isTracking Whether to enable or disable memory tracking.
         */
        void setIsMemoryTracking(b8 isTracking) const;

        /**
         * @brief Registers the current thread for metrics.
         * @param alias The alias to use for this thread.
         */
        void registerThread(const std::string& alias);

        /**
         * @brief Unregisters the current thread from metrics.
         */
        void unregisterThread();

        /**
         * @brief Checks if the given thread is registered for metrics.
         * @param thread The thread to check.
         * @return Whether the thread is registered.
         */
        b8 isRegistered(const UUID& thread) const;
        /**
         * @brief Checks if the current thread is registered for metrics.
         * @return Whether the thread is registered.
         */
        b8 isRegistered() const;

        private:
        struct ThreadMetrics {
            rome::u64 currentBytes = 0;                        ///< The current total heap-allocated bytes.
            rome::u64 peakBytes = 0;                           ///< The maximum number of bytes allocated during program execution.
            rome::u64 totalBytes = 0;                          ///< The total number of bytes allocated during program execution.
            rome::u64 totalAllocations = 0;                    ///< The total number of heap allocations.
            rome::b8 memoryLogging = false;                    ///< Whether to log memory allocation and deallocation.
            std::string alias = "Main";                        ///< The alias for this thread.
            std::unordered_map<void*, rome::u64> allocations;  ///< Tracks each pointer's allocated size.
        };

        mutable std::mutex registrarMutex;                       ///< Protects allocations and the counters from concurrent access.
        std::unordered_map<UUID, ThreadMetrics*> threadMetrics;  ///< The metrics for each thread.
    };
}  // namespace rome::core