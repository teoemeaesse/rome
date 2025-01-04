#pragma once

#include <chrono>

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief A simple timer class meant to measure time between two points.
     */
    class Timer {
        public:
        Timer() = default;
        ~Timer() = default;

        /**
         * @brief Starts the timer.
         */
        void start();

        /**
         * @brief Resets the timer and returns the time elapsed since the last start / tick.
         * @return The time elapsed in microseconds.
         */
        u64 tick();

        private:
        std::chrono::steady_clock clock;
        u64 lastTime;
    };
}  // namespace iodine::core