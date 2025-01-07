#pragma once

#include <mutex>
#include <unordered_map>

#include "debug/log.h"

namespace iodine::core {
    static inline std::atomic_bool metricsShutdown = false;  ///< Whether the metrics have been shut down. This only happens on program exit.
    /**
     * @brief Tracks program time and memory performance over time.
     */
    class Metrics {
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
         * @brief Registers a heap memory allocation.
         * @param ptr The pointer returned by the underlying allocation.
         * @param size The number of bytes requested.
         * @note This action is thread-local.
         */
        void registerAllocation(void* ptr, u64 size);

        /**
         * @brief Registers a heap memory free operation.
         * @param ptr The pointer being freed.
         * @note This action is thread-local.
         */
        void registerDeallocation(void* ptr);

        /**
         * @brief Logs the current metrics for all threads.
         */
        void report() const;

        using ThreadId = i32;

        /**
         * @brief Gets a string representation of the memory metrics for this thread.
         * @param thread The thread to get the memory metrics for.
         * @return The memory metrics as a string.
         * @note This action is thread-local.
         */
        std::string getMemoryMetrics(ThreadId thread) const;
        /**
         * @brief Gets a string representation of the memory metrics for this thread.
         * @return The memory metrics as a string.
         */
        std::string getMemoryMetrics() const;

        /**
         * @brief Gets the total number of bytes currently heap-allocated by this thread.
         * @param thread The thread to get the memory metrics for.
         * @return The number of bytes currently allocated.
         * @note This action is thread-local.
         */
        u64 getCurrentBytes(ThreadId thread) const;
        /**
         * @brief Gets the total number of bytes currently heap-allocated by this thread.
         * @return The number of bytes currently allocated.
         */
        u64 getCurrentBytes() const;

        /**
         * @brief Gets the peak number of bytes that have been allocated during program execution by this thread.
         * @param thread The thread to get the memory metrics for.
         * @return The peak number of bytes allocated.
         * @note This action is thread-local.
         */
        u64 getPeakBytes(ThreadId thread) const;
        /**
         * @brief Gets the peak number of bytes that have been allocated during program execution by this thread.
         * @return The peak number of bytes allocated.
         */
        u64 getPeakBytes() const;

        /**
         * @brief Gets the total number of bytes allocated during program execution by this thread.
         * @param thread The thread to get the memory metrics for.
         * @return The total number of bytes allocated.
         * @note This action is thread-local.
         */
        u64 getTotalBytes(ThreadId thread) const;
        /**
         * @brief Gets the total number of bytes allocated during program execution by this thread.
         * @return The total number of bytes allocated.
         */
        u64 getTotalBytes() const;

        /**
         * @brief Gets the total number of heap allocations during program execution by this thread.
         * @param thread The thread to get the memory metrics for.
         * @return The total number of heap allocations.
         * @note This action is thread-local.
         */
        u64 getTotalAllocations(ThreadId thread) const;
        /**
         * @brief Gets the total number of heap allocations during program execution by this thread.
         * @return The total number of heap allocations.
         */
        u64 getTotalAllocations() const;

        /**
         * @brief Gets the number of heap allocations that have not been deallocated by this thread.
         * @param thread The thread to get the memory metrics for.
         * @return The number of missing deallocations.
         * @note This action is thread-local.
         */
        u64 getMissingDeallocations(ThreadId thread) const;
        /**
         * @brief Gets the number of heap allocations that have not been deallocated by this thread.
         * @return The number of missing deallocations.
         */
        u64 getMissingDeallocations() const;

        /**
         * @brief Gets whether memory tracking is enabled for this thread.
         * @param thread The thread to check.
         * @return Whether memory tracking is enabled.
         * @note This value is thread-local.
         */
        b8 isMemoryTracking(ThreadId thread) const;

        /**
         * @brief Enables / disables memory tracking for this thread.
         * @param thread The thread to enable / disable memory tracking for.
         * @note This action is thread-local.
         */
        void setIsMemoryTracking(ThreadId thread, b8 isTracking) const;
        /**
         * @brief Enables / disables memory tracking for this thread.
         */
        void setIsMemoryTracking(b8 isTracking) const;

        /**
         * @brief Registers the current thread for metrics.
         * @return This thread's ID.
         * @note This action is thread-local.
         */
        ThreadId registerThread();

        /**
         * @brief Unregisters the current thread from metrics.
         * @note This action is thread-local.
         */
        void unregisterThread();

        private:
        struct ThreadMetrics {
            iodine::u64 currentBytes = 0;                        ///< The current total heap-allocated bytes.
            iodine::u64 peakBytes = 0;                           ///< The maximum number of bytes allocated during program execution.
            iodine::u64 totalBytes = 0;                          ///< The total number of bytes allocated during program execution.
            iodine::u64 totalAllocations = 0;                    ///< The total number of heap allocations.
            iodine::b8 memoryLogging = false;                    ///< Whether to log memory allocation and deallocation.
            std::unordered_map<void*, iodine::u64> allocations;  ///< Tracks each pointer's allocated size.
        };

        mutable std::mutex registrarMutex;                           ///< Protects allocations and the counters from concurrent access.
        std::unordered_map<ThreadId, ThreadMetrics*> threadMetrics;  ///< The metrics for each thread.
        i32 nextThread = 0;                                          ///< The next thread ID to assign.
    };
}  // namespace iodine::core