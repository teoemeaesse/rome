#pragma once

#include <thread>

#include "debug/metrics.h"

namespace iodine::core {
    class Thread {
        public:
        Thread() = default;
        ~Thread();
        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;
        Thread(Thread&& other) noexcept;
        Thread& operator=(Thread&& other) noexcept;

        /**
         * @brief Begins thread execution.
         * @tparam Function The function type to run in the thread.
         * @tparam ...Args The types of arguments to pass to the function.
         * @param function The function to run in the thread.
         * @param ...args The arguments to pass to the function.s
         */
        template <typename Function, typename... Args>
        void run(Function&& function, Args&&... args) {
            start(std::forward<Function>(function), std::forward<Args>(args)...);
        }

        /**
         * @brief Joins the thread, blocking until it completes.
         */
        void join();

        /**
         * @brief Detaches the thread, allowing it to run in the background.
         */
        void detach();

        /**
         * @brief Sets whether memory tracking is enabled for this thread. False by default.
         * @param isMemoryTracking Whether memory tracking is enabled.
         */
        inline void setIsMemoryTracking(b8 isMemoryTracking) {
            isMetricsStateChanged = true;
            this->isMemoryTracking = isMemoryTracking;
        }
        /**
         * @brief Sets whether performance tracking is enabled for this thread. False by default.
         * @param isPerformanceTracking Whether performance tracking is enabled.
         */
        inline void setIsPerformanceTracking(b8 isPerformanceTracking) {
            isMetricsStateChanged = true;
            this->isPerformanceTracking = isPerformanceTracking;
        }

        /**
         * @brief Gets the thread id.
         * @return The thread id.
         */
        inline std::thread::id getThreadId() const { return thread.get_id(); }

        /**
         * @brief Gets the metrics id for this thread.
         * @return The metrics id.
         */
        inline Metrics::ThreadId getMetricsId() const { return metricsId; }

        /**
         * @brief Checks whether the thread is running.
         * @return Whether the thread is running.
         */
        inline b8 isRunning() const { return thread.joinable(); }

        private:
        std::thread thread{};              ///< The thread object.
        Metrics::ThreadId metricsId;       ///< The metrics id for this thread.
        b8 isMemoryTracking = false;       ///< Whether memory tracking is enabled.
        b8 isPerformanceTracking = false;  ///< Whether performance tracking is enabled.
        b8 isMetricsStateChanged = false;  ///< Whether the metrics state has changed.

        /**
         * @brief Starts the thread after registering it with the metrics tracker.
         * @tparam Function The function type to run in the thread.
         * @tparam ...Args The types of arguments to pass to the function.
         * @param function The function to run in the thread.
         * @param ...args The arguments to pass to the function.
         */
        template <typename Function, typename... Args>
        void start(Function&& function, Args&&... args) {
            metricsId = Metrics::getInstance().registerThread();
            updateMetrics();
            thread = std::thread(std::forward<Function>(function), std::forward<Args>(args)...);
        }

        /**
         * @brief Checks for updates to the metrics state for this thread and propagates them accordingly.
         * @note If you intend to change the tracking settings for this thread after starting it, call this function from the thread's function.
         */
        void updateMetrics();
    };
}  // namespace iodine::core