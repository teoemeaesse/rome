#include "debug/metrics.h"

#include <cstdlib>
#include <new>

void* operator new(size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }

    iodine::core::Metrics::getInstance().registerAllocation(ptr, static_cast<iodine::u64>(size));

    return ptr;
}
void operator delete(void* ptr) noexcept {
    iodine::core::Metrics::getInstance().registerDeallocation(ptr);
    std::free(ptr);
}

void* operator new[](size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }

    iodine::core::Metrics::getInstance().registerAllocation(ptr, size);

    return ptr;
}

void operator delete[](void* ptr) noexcept {
    iodine::core::Metrics::getInstance().registerDeallocation(ptr);
    std::free(ptr);
}

namespace iodine::core {
    void Metrics::registerAllocation(void* ptr, u64 size) {
        if (!ptr) return;

        std::lock_guard<std::mutex> lock(registrarMutex);
        allocations[ptr] = size;
        currentBytes += size;
        totalBytes += size;
        totalAllocations++;
        if (currentBytes > peakBytes) {
            peakBytes = currentBytes;
        }
    }

    /**
     * @brief Registers a heap memory free operation.
     * @param ptr The pointer being freed.
     */
    void Metrics::registerDeallocation(void* ptr) {
        if (!ptr) {
            IO_WARN("Attempted to deallocate a null pointer");
            return;
        }

        std::lock_guard<std::mutex> lock(registrarMutex);
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            u64 size = it->second;
            currentBytes -= size;
            allocations.erase(it);
        }

        IO_WARNV("Attempted to deallocate a pointer that was not allocated by the Iodine runtime: %p", ptr);
    }

    std::string Metrics::getMemoryMetrics() const {
        std::lock_guard<std::mutex> lock(registrarMutex);
        return "Heap memory metrics:\n"
               " - Total:            " +
               std::to_string(totalBytes) + "B\n - Peak:             " + std::to_string(peakBytes) +
               "B\n - Current / leaked: " + std::to_string(currentBytes) + "B\n - Total allocations: " + std::to_string(totalAllocations) +
               "\n - Total deallocations: " + std::to_string(totalAllocations - getMissingDeallocations());
    }
}  // namespace iodine::core