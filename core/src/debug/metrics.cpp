#include "debug/metrics.hpp"

#include <new>

#include "debug/exception.hpp"

static std::atomic_bool metricsShutdown = false;  ///< Whether the metrics have been shut down. This only happens on program exit.
static thread_local iodine::core::Metrics::ThreadId threadId =
    -1;                                                 ///< The ID of the current thread. Assigned on registration and indexes into threadMetrics.
static thread_local std::string threadAlias = "Main";   ///< The alias of the current thread. Assigned on registration and used for logging.
static thread_local iodine::b8 recursionGuard = false;  ///< Used to prevent circular new / delete calls.

void* operator new(size_t size) {
    if (threadId == -1 || metricsShutdown) {
        return std::malloc(size);
    }
    if (!recursionGuard) {
        recursionGuard = true;
        void* ptr = std::malloc(size);
        if (!ptr) {
            recursionGuard = false;
            throw std::bad_alloc();
        }

        iodine::core::Metrics::getInstance().registerAllocation(ptr, static_cast<iodine::u64>(size));
        recursionGuard = false;
        return ptr;
    }
    return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
    if (threadId == -1 || metricsShutdown) {
        std::free(ptr);
        return;
    }
    if (!recursionGuard) {
        recursionGuard = true;
        iodine::core::Metrics::getInstance().registerDeallocation(ptr);
        recursionGuard = false;
    }
    std::free(ptr);
}

void* operator new[](size_t size) {
    if (threadId == -1 || metricsShutdown) {
        return std::malloc(size);
    }
    if (!recursionGuard) {
        recursionGuard = true;
        void* ptr = std::malloc(size);
        if (!ptr) {
            recursionGuard = false;
            throw std::bad_alloc();
        }

        iodine::core::Metrics::getInstance().registerAllocation(ptr, static_cast<iodine::u64>(size));
        recursionGuard = false;
        return ptr;
    }

    return std::malloc(size);
}

void operator delete[](void* ptr) noexcept {
    if (threadId == -1 || metricsShutdown) {
        std::free(ptr);
        return;
    }
    if (!recursionGuard) {
        recursionGuard = true;
        iodine::core::Metrics::getInstance().registerDeallocation(ptr);
        recursionGuard = false;
    }
    std::free(ptr);
}

namespace iodine::core {
    Metrics::~Metrics() {
        metricsShutdown = true;
        for (const auto& [thread, metrics] : threadMetrics) {
            delete metrics;
        }
    }

    void Metrics::registerAllocation(void* ptr, u64 size) {
        if (!isMemoryTracking(threadId)) return;

        if (!ptr) return;

        std::lock_guard<std::mutex> lock(registrarMutex);
        ThreadMetrics* metrics = threadMetrics[threadId];
        metrics->allocations[ptr] = size;
        metrics->currentBytes += size;
        metrics->totalBytes += size;
        metrics->totalAllocations++;
        if (metrics->currentBytes > metrics->peakBytes) {
            metrics->peakBytes = metrics->currentBytes;
        }
    }

    /**
     * @brief Registers a heap memory free operation.
     * @param ptr The pointer being freed.
     */
    void Metrics::registerDeallocation(void* ptr) {
        if (!isMemoryTracking(threadId)) return;

        if (!ptr) {
            IO_WARN("Attempted to deallocate a null pointer");
            return;
        }

        std::lock_guard<std::mutex> lock(registrarMutex);
        ThreadMetrics* metrics = threadMetrics[threadId];
        auto it = metrics->allocations.find(ptr);
        if (it != metrics->allocations.end()) {
            u64 size = it->second;
            metrics->currentBytes -= size;
            metrics->allocations.erase(it);
            return;
        }
    }

    void Metrics::report() const {
        IO_INFO("Memory metrics:");
        for (const auto& [thread, metrics] : threadMetrics) {
            IO_INFO(getMemoryMetrics(thread).c_str());
        }
        IO_INFO(getMemoryMetrics().c_str());
    }

    std::string Metrics::getMemoryMetrics(ThreadId thread) const {
        return "Thread (\"" + getThreadAlias(thread) + "\") heap metrics:\n          - Total               " + std::to_string(getTotalBytes(thread)) +
               " B\n          - Peak                " + std::to_string(getPeakBytes(thread)) + " B\n          - Current / leaked    " +
               std::to_string(getCurrentBytes(thread)) + " B\n          - Total allocations   " + std::to_string(getTotalAllocations(thread)) +
               "\n          - Total deallocations " + std::to_string(getTotalAllocations(thread) - getMissingDeallocations(thread));
    }
    std::string Metrics::getMemoryMetrics() const {
        struct {
            iodine::u64 currentBytes = 0;
            iodine::u64 peakBytes = 0;
            iodine::u64 totalBytes = 0;
            iodine::u64 totalAllocations = 0;
            iodine::u64 missingDeallocations = 0;
        } globalMetrics;
        for (const auto& [thread, metrics] : threadMetrics) {
            globalMetrics.currentBytes += metrics->currentBytes;
            globalMetrics.peakBytes += metrics->peakBytes;
            globalMetrics.totalBytes += metrics->totalBytes;
            globalMetrics.totalAllocations += metrics->totalAllocations;
            globalMetrics.missingDeallocations += threadMetrics.at(thread)->allocations.size();
        }
        return "Global heap metrics:\n          - Total               " + std::to_string(globalMetrics.totalBytes) +
               " B\n          - Peak                " + std::to_string(globalMetrics.peakBytes) + " B\n          - Current / leaked    " +
               std::to_string(globalMetrics.currentBytes) + " B\n          - Total allocations   " + std::to_string(globalMetrics.totalAllocations) +
               "\n          - Total deallocations " + std::to_string(globalMetrics.totalAllocations - globalMetrics.missingDeallocations);
    }

    const std::string& Metrics::getThreadAlias(ThreadId threadId) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(threadId)->threadAlias;
    }

    const std::string& Metrics::getThreadAlias() const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadAlias;
    }

    u64 Metrics::getCurrentBytes(ThreadId thread) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->currentBytes;
    }
    u64 Metrics::getCurrentBytes() const {
        u64 currentBytes = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            currentBytes += threadMetrics.at(thread)->currentBytes;
        }
        return currentBytes;
    }

    u64 Metrics::getPeakBytes(ThreadId thread) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->peakBytes;
    }
    u64 Metrics::getPeakBytes() const {
        u64 peakBytes = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            peakBytes += threadMetrics.at(thread)->peakBytes;
        }
        return peakBytes;
    }

    u64 Metrics::getTotalBytes(ThreadId thread) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->totalBytes;
    }
    u64 Metrics::getTotalBytes() const {
        u64 totalBytes = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            totalBytes += threadMetrics.at(thread)->totalBytes;
        }
        return totalBytes;
    }

    u64 Metrics::getTotalAllocations(ThreadId thread) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->totalAllocations;
    }
    u64 Metrics::getTotalAllocations() const {
        u64 totalAllocations = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            totalAllocations += threadMetrics.at(thread)->totalAllocations;
        }
        return totalAllocations;
    }

    u64 Metrics::getMissingDeallocations(ThreadId thread) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->allocations.size();
    }
    u64 Metrics::getMissingDeallocations() const {
        u64 missingDeallocations = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            missingDeallocations += threadMetrics.at(thread)->allocations.size();
        }
        return missingDeallocations;
    }

    b8 Metrics::isMemoryTracking(ThreadId thread) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->memoryLogging;
    }
    b8 Metrics::isMemoryTracking() const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(threadId)->memoryLogging;
    }

    void Metrics::setIsMemoryTracking(ThreadId thread, b8 isTracking) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        threadMetrics.at(thread)->memoryLogging = isTracking;
    }
    void Metrics::setIsMemoryTracking(b8 isTracking) const {
        if (threadId == -1 || threadMetrics.find(threadId) == threadMetrics.end()) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        for (const auto& [thread, metrics] : threadMetrics) {
            threadMetrics.at(thread)->memoryLogging = isTracking;
        }
    }

    Metrics::ThreadId Metrics::registerThread(const std::string& alias) {
        std::lock_guard<std::mutex> lock(registrarMutex);
        if (threadId != -1) {
            IO_WARN("Thread already registered");
            return threadId;
        }

        threadId = static_cast<ThreadId>(nextThread++);
        threadAlias = alias;
        threadMetrics[threadId] = new ThreadMetrics();
        threadMetrics[threadId]->threadAlias = alias;
        return threadId;
    }

    void Metrics::unregisterThread() {
        std::lock_guard<std::mutex> lock(registrarMutex);
        if (threadId == -1) {
            IO_WARN("Thread not registered");
            return;
        }

        delete threadMetrics[threadId];
        threadMetrics.erase(threadId);
        threadId = -1;
    }
}  // namespace iodine::core