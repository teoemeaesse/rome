#include "chrono/rate.hpp"

namespace iodine::core {
    RateTracker::RateTracker(u32 target, f64 window) : target(target), count(0), elapsed(0), window(window), estimate(0) {}

    void RateTracker::tick(f64 delta) {
        elapsed += delta;
        count++;
        if (elapsed >= window) {
            estimate = static_cast<u32>(count / elapsed);
            count = 0;
            elapsed = 0;
        }
    }
}  // namespace iodine::core