#pragma once

#include <functional>

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief Defines how an application should tick and render.
     * @note Subclass this to configure your own game loop.
     */
    class ApplicationStrategy {
        public:
        /**
         * @brief Creates a new application strategy.
         * @param tick The tick function.
         * @param render The render function.
         */
        ApplicationStrategy(const std::function<void(f64)>& tick, const std::function<void(f64)>& render);
        virtual ~ApplicationStrategy() = default;

        /**
         * @brief Runs the application loop. Your game loop logic goes here.
         * @param tickRate The tick rate.
         * @param renderRate The render rate.
         */
        virtual void run(f64 tickRate, f64 renderRate) = 0;

        /**
         * @brief Starts / resumes the application loop.
         * @param tickRate The tick rate.
         * @param renderRate The render rate.
         */
        void start(f64 tickRate, f64 renderRate);

        /**
         * @brief Pauses ticking execution.
         */
        void pause();

        /**
         * @brief Stops the application loop.
         */
        void stop();

        protected:
        /**
         * @brief The status of the application loop.
         */
        enum class Status {
            Ok,     ///< The application is running.
            Error,  ///< An error occurred.
            Pause,  ///< The application is paused - render but don't tick.
            Done    ///< The application has finished.
        };
        Status status = Status::Done;  ///< The status of the application loop.

        const std::function<void(f64)> tick;    ///< The tick function.
        const std::function<void(f64)> render;  ///< The render function;
    };
}  // namespace iodine::core