#pragma once

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief The base class for all iodine applications.
     */
    class Application {
        /**
         * @brief The status of the application.
         */
        enum class Status {
            Ok,     ///< The application is running.
            Error,  ///< An error occurred.
            Pause,  ///< The application is paused - render but don't tick.
            Stop,   ///< The application has stopped.
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

        public:
        /**
         * @brief Creates a new application.
         * @param framerate The target framerate for the application.
         * @return The application.
         */
        Application(u32 framerate = 60);
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

        private:
        Status status;    ///< The status of the application.
        Metrics metrics;  ///< The performance metrics of the application.

        /**
         * @brief The main game loop. Calls tick() and render() in a loop.
         */
        void loop();
    };

    /**
     * @brief Creates a new application. This function must be implemented by the user.
     * @return A pointer to the application.
     */
    Application* createApplication();
}  // namespace iodine::core