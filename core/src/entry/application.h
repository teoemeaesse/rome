#pragma once

#include <string>

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief The base class for all iodine applications.
     */
    class Application {
        public:
        struct Config;

        /**
         * @brief Creates a new application.
         * @param config The configuration for the application.
         * @return The application.
         */
        explicit Application(const Config& config);
        virtual ~Application() = default;

        /**
         * @brief Initializes the application.
         */
        virtual void setup() = 0;

        /**
         * @brief Runs at a fixed time step.
         */
        virtual void tick() = 0;

        /**
         * @brief Runs as fast as possible.
         * @param delta The time since the last frame.
         */
        virtual void render(f32 delta) = 0;

        /**
         * @brief Starts or resumes the game loop.
         */
        void start();
        /**
         * @brief Pauses the game loop.
         */
        void pause();
        /**
         * @brief Stops the game loop.
         */
        void stop();
        /**
         * @brief Finishes the application.
         */
        void finish();

        /**
         * @brief Configuration for the application.
         */
        struct Config {
            std::string title = "Iodine";     ///< The title of the application. Window title should default to this.
            u32 framerate = 60;               ///< The target framerate of the application.
            b8 isMemoryLogging = false;       ///< Whether to log memory allocations.
            b8 isPerformanceLogging = false;  ///< Whether to log performance metrics.
        };

        /**
         * @brief A builder for the application configuration.
         */
        class Builder {
            public:
            Builder& setTitle(const std::string& title) {
                config.title = title;
                return *this;
            }

            Builder& setFramerate(u32 framerate) {
                config.framerate = framerate;
                return *this;
            }

            Builder& enableMemoryLogging() {
                config.isMemoryLogging = true;
                return *this;
            }

            Builder& enablePerformanceLogging() {
                config.isPerformanceLogging = true;
                return *this;
            }

            Config build() { return config; }

            private:
            Config config;
        };

        /**
         * @brief The status of the application.
         */
        enum class Status {
            Ok,     ///< The application is running.
            Error,  ///< An error occurred.
            Pause,  ///< The application is paused - render but don't tick.
            Done    ///< The application has finished.
        };

        /**
         * @brief Performance metrics for the application.
         */
        struct Metrics {
            u32 framerateTarget;    ///< The target framerate.
            u32 framerateEstimate;  ///< The estimated framerate.
            f32 framerateWindow;    ///< How long to average the framerate over (in seconds).
            u32 frameCount;         ///< The number of frames rendered within the window.
            f32 frameTime;          ///< The time taken to render a frame.
        };

        private:
        Config config;
        Status status;    ///< The status of the application.
        Metrics metrics;  ///< The performance metrics of the application. PLACEHOLDER

        /**
         * @brief The main game loop. Calls tick() and render() in a loop.
         */
        void loop();
    };

}  // namespace iodine::core

/**
 * @brief Creates a new application. This function must be implemented by the user.
 * @return A pointer to the application.
 */
iodine::core::Application* createApplication();