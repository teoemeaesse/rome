#include "thread/thread.h"

namespace iodine::core {
    Thread::~Thread() { join(); }

    Thread::Thread(Thread&& other) noexcept : thread(std::move(other.thread)) {}

    Thread& Thread::operator=(Thread&& other) noexcept {
        if (this != &other) {
            if (thread.joinable()) {
                thread.join();  // Join current thread before moving
            }
            thread = std::move(other.thread);
        }
        return *this;
    }

    void Thread::join() {
        if (thread.joinable()) {
            thread.join();
        }
    }

    void Thread::detach() {
        if (thread.joinable()) {
            thread.detach();
        }
    }

    void Thread::updateMetrics() {
        if (isMetricsStateChanged) {
            Metrics::getInstance().setIsMemoryTracking(metricsId, isMemoryTracking);
            // Metrics::getInstance().setIsPerformanceTracking(metricsId, isPerformanceTracking); //TODO: Implement
        }
    }
}  // namespace iodine::core