#pragma once

#include "prelude.hpp"

namespace rome::core {
    /**
     * @brief A simple timer class meant to measure time between two points.
     */
    class RM_API Timer {
        public:
        Timer() = default;
        ~Timer() = default;

        /**
         * @brief Starts the timer.
         */
        void start();

        /**
         * @brief Resets the timer and returns the time elapsed since the last start / tick.
         * @return The time elapsed in seconds.
         */
        f64 tick();

        private:
        f64 lastTime;
    };
}  // namespace rome::core