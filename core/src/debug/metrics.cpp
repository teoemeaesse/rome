#include "debug/metrics.h"

#include <new>

static thread_local bool recursionGuard = false;  ///< Used to prevent circular new / delete calls.

void* operator new(size_t size) {
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
    if (!recursionGuard) {
        recursionGuard = true;
        iodine::core::Metrics::getInstance().registerDeallocation(ptr);
        recursionGuard = false;
    }
    std::free(ptr);
}

void* operator new[](size_t size) {
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
    if (!recursionGuard) {
        recursionGuard = true;
        iodine::core::Metrics::getInstance().registerDeallocation(ptr);
        recursionGuard = false;
    }
    std::free(ptr);
}

namespace iodine::core {
    void Metrics::registerAllocation(void* ptr, u64 size) {
        if (!memoryLogging) return;

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
        if (!memoryLogging) return;

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
            return;
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