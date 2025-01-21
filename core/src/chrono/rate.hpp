#pragma once

#include "prelude.hpp"

namespace iodine::core {
    /**
     * @brief Tracks the running rate of an application.
     */
    class IO_API RateTracker {
        public:
        /**
         * @brief Creates a new rate tracker.
         * @param target The target rate.
         * @param window The window to average the rate over (in seconds).
         */
        RateTracker(u32 target, f64 window);
        ~RateTracker() = default;

        /**
         * @brief Registers that a tick has occurred.
         * @param delta The time since the last tick (in seconds).
         */
        void tick(f64 delta);

        /**
         * @brief Gets the latest estimated rate.
         * @return The rate.
         */
        inline f64 getRate() const { return estimate; }

        private:
        u32 target;   ///< The target rate.
        u32 count;    ///< The number of ticks within the window.
        f64 elapsed;  ///< The total time elapsed within the window.
        f64 window;   ///< How long to average the rate over (in seconds).

        f64 estimate;  ///< The latest estimated rate.
    };
}  // namespace iodine::core