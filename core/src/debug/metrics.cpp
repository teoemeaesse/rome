#include "debug/metrics.hpp"

#include <new>

#include "concurrency/thread.hpp"
#include "debug/exception.hpp"

static std::atomic_bool metricsRunning = false;       ///< Whether the metrics system should be logging performance data.
static thread_local rome::b8 recursionGuard = false;  ///< Used to prevent circular new / delete calls.

void* operator new(size_t size) {
    if (!metricsRunning) {
        return std::malloc(size);
    }
    if (!rome::core::Metrics::getInstance().isRegistered() || !rome::core::Metrics::getInstance().isMemoryTracking()) {
        return std::malloc(size);
    }
    if (!recursionGuard) {
        recursionGuard = true;
        void* ptr = std::malloc(size);
        if (!ptr) {
            recursionGuard = false;
            throw std::bad_alloc();
        }

        rome::core::Metrics::getInstance().registerAllocation(ptr, static_cast<rome::u64>(size));
        recursionGuard = false;
        return ptr;
    }
    return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
    if (!metricsRunning) {
        std::free(ptr);
        return;
    }
    if (!rome::core::Metrics::getInstance().isRegistered() || !rome::core::Metrics::getInstance().isMemoryTracking()) {
        std::free(ptr);
        return;
    }
    if (!recursionGuard) {
        recursionGuard = true;
        rome::core::Metrics::getInstance().registerDeallocation(ptr);
        recursionGuard = false;
    }
    std::free(ptr);
}

void* operator new[](size_t size) {
    if (!metricsRunning) {
        return std::malloc(size);
    }
    if (!rome::core::Metrics::getInstance().isRegistered() || !rome::core::Metrics::getInstance().isMemoryTracking()) {
        return std::malloc(size);
    }
    if (!recursionGuard) {
        recursionGuard = true;
        void* ptr = std::malloc(size);
        if (!ptr) {
            recursionGuard = false;
            throw std::bad_alloc();
        }

        rome::core::Metrics::getInstance().registerAllocation(ptr, static_cast<rome::u64>(size));
        recursionGuard = false;
        return ptr;
    }

    return std::malloc(size);
}

void operator delete[](void* ptr) noexcept {
    if (!metricsRunning) {
        std::free(ptr);
        return;
    }
    if (!rome::core::Metrics::getInstance().isRegistered() || !rome::core::Metrics::getInstance().isMemoryTracking()) {
        std::free(ptr);
        return;
    }
    if (!recursionGuard) {
        recursionGuard = true;
        rome::core::Metrics::getInstance().registerDeallocation(ptr);
        recursionGuard = false;
    }
    std::free(ptr);
}

namespace rome::core {
    Metrics::~Metrics() {
        metricsRunning = false;
        for (const auto& [thread, metrics] : threadMetrics) {
            delete metrics;
        }
    }

    void Metrics::start() {
        RM_DEBUG("Starting metrics");
        metricsRunning = true;
        RM_DEBUG("Metrics started");
    }

    void Metrics::stop() { metricsRunning = false; }

    void Metrics::registerAllocation(void* ptr, u64 size) {
        if (!ptr) return;

        std::lock_guard<std::mutex> lock(registrarMutex);
        ThreadMetrics* metrics = threadMetrics[ThreadInfo::getLocalID()];
        metrics->allocations[ptr] = size;
        metrics->currentBytes += size;
        metrics->totalBytes += size;
        metrics->totalAllocations++;
        if (metrics->currentBytes > metrics->peakBytes) {
            metrics->peakBytes = metrics->currentBytes;
        }
    }

    void Metrics::registerDeallocation(void* ptr) {
        if (!ptr) {
            RM_WARN("Attempted to deallocate a null pointer");
            return;
        }

        std::lock_guard<std::mutex> lock(registrarMutex);
        ThreadMetrics* metrics = threadMetrics[ThreadInfo::getLocalID()];
        auto it = metrics->allocations.find(ptr);
        if (it != metrics->allocations.end()) {
            u64 size = it->second;
            metrics->currentBytes -= size;
            metrics->allocations.erase(it);
            return;
        }
    }

    void Metrics::report() const {
        RM_INFO("Memory metrics:");
        for (const auto& [thread, metrics] : threadMetrics) {
            RM_INFO(getMemoryMetrics(thread).c_str());
        }
        RM_INFO(getGlobalMemoryMetrics().c_str());
    }

    std::string Metrics::getMemoryMetrics(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return "Thread (\"" + threadMetrics.at(thread)->alias + "\") heap metrics:\n          - Total               " +
               std::to_string(getTotalBytes(thread)) + " B\n          - Peak                " + std::to_string(getPeakBytes(thread)) +
               " B\n          - Current / leaked    " + std::to_string(getCurrentBytes(thread)) + " B\n          - Total allocations   " +
               std::to_string(getTotalAllocations(thread)) + "\n          - Total deallocations " +
               std::to_string(getTotalAllocations(thread) - getMissingDeallocations(thread));
    }

    std::string Metrics::getMemoryMetrics() const { return getMemoryMetrics(ThreadInfo::getLocalID()); }

    std::string Metrics::getGlobalMemoryMetrics() const {
        return "Global heap metrics:\n          - Total               " + std::to_string(getGlobalTotalBytes()) +
               " B\n          - Peak                " + std::to_string(getGlobalPeakBytes()) + " B\n          - Current / leaked    " +
               std::to_string(getGlobalCurrentBytes()) + " B\n          - Total allocations   " + std::to_string(getGlobalTotalAllocations()) +
               "\n          - Total deallocations " + std::to_string(getGlobalTotalAllocations() - getGlobalMissingDeallocations());
    }

    const std::string& Metrics::getThreadAlias(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->alias;
    }

    const std::string& Metrics::getThreadAlias() const { return getThreadAlias(ThreadInfo::getLocalID()); }

    u64 Metrics::getCurrentBytes(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->currentBytes;
    }

    u64 Metrics::getCurrentBytes() const { return getCurrentBytes(ThreadInfo::getLocalID()); }

    u64 Metrics::getGlobalCurrentBytes() const {
        u64 currentBytes = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            currentBytes += threadMetrics.at(thread)->currentBytes;
        }
        return currentBytes;
    }

    u64 Metrics::getPeakBytes(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->peakBytes;
    }

    u64 Metrics::getPeakBytes() const { return getPeakBytes(ThreadInfo::getLocalID()); }

    u64 Metrics::getGlobalPeakBytes() const {
        u64 peakBytes = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            peakBytes += threadMetrics.at(thread)->peakBytes;
        }
        return peakBytes;
    }

    u64 Metrics::getTotalBytes(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->totalBytes;
    }

    u64 Metrics::getTotalBytes() const { return getTotalBytes(ThreadInfo::getLocalID()); }

    u64 Metrics::getGlobalTotalBytes() const {
        u64 totalBytes = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            totalBytes += threadMetrics.at(thread)->totalBytes;
        }
        return totalBytes;
    }

    u64 Metrics::getTotalAllocations(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->totalAllocations;
    }

    u64 Metrics::getTotalAllocations() const { return getTotalAllocations(ThreadInfo::getLocalID()); }

    u64 Metrics::getGlobalTotalAllocations() const {
        u64 totalAllocations = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            totalAllocations += threadMetrics.at(thread)->totalAllocations;
        }
        return totalAllocations;
    }

    u64 Metrics::getMissingDeallocations(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->allocations.size();
    }

    u64 Metrics::getMissingDeallocations() const { return getMissingDeallocations(ThreadInfo::getLocalID()); }

    u64 Metrics::getGlobalMissingDeallocations() const {
        u64 missingDeallocations = 0;
        for (const auto& [thread, metrics] : threadMetrics) {
            missingDeallocations += threadMetrics.at(thread)->allocations.size();
        }
        return missingDeallocations;
    }

    b8 Metrics::isMemoryTracking(const UUID& thread) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(thread)->memoryLogging;
    }

    b8 Metrics::isMemoryTracking() const {
        if (!isRegistered(ThreadInfo::getLocalID())) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        return threadMetrics.at(ThreadInfo::getLocalID())->memoryLogging;
    }

    void Metrics::setIsMemoryTracking(const UUID& thread, b8 isTracking) const {
        if (!isRegistered(thread)) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        threadMetrics.at(thread)->memoryLogging = isTracking;
    }

    void Metrics::setIsMemoryTracking(b8 isTracking) const {
        if (!isRegistered(ThreadInfo::getLocalID())) {
            THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Thread ID not registered");
        }
        threadMetrics.at(ThreadInfo::getLocalID())->memoryLogging = isTracking;
    }

    void Metrics::registerThread(const std::string& alias) {
        std::lock_guard<std::mutex> lock(registrarMutex);
        if (isRegistered(ThreadInfo::getLocalID())) {
            RM_WARN("Thread already registered");
            return;
        }

        threadMetrics[ThreadInfo::getLocalID()] = new ThreadMetrics();
        threadMetrics[ThreadInfo::getLocalID()]->alias = alias;
    }

    void Metrics::unregisterThread() {
        std::lock_guard<std::mutex> lock(registrarMutex);
        if (!isRegistered(ThreadInfo::getLocalID())) {
            RM_WARN("Thread not registered");
            return;
        }

        delete threadMetrics[ThreadInfo::getLocalID()];
        threadMetrics.erase(ThreadInfo::getLocalID());
    }

    b8 Metrics::isRegistered(const UUID& thread) const { return threadMetrics.find(thread) != threadMetrics.end(); }

    b8 Metrics::isRegistered() const { return threadMetrics.find(ThreadInfo::getLocalID()) != threadMetrics.end(); }
}  // namespace rome::core