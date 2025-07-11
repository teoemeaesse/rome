#pragma once

#include "app/strategy.hpp"
#include "concurrency/thread.hpp"

namespace rome::core {
    class Application;

    /**
     * @brief An application strategy consisting of one tick thread and one render thread.
     */
    class RM_API TwinStrategy : public ApplicationStrategy {
        public:
        TwinStrategy(Application& app, b8 memoryMetrics = true);
        ~TwinStrategy() override = default;

        /**
         * @brief Runs the application loop.
         * @param tickRate The tick rate.
         * @param renderRate The render rate.
         */
        void run(f64 tickRate, f64 renderRate) override;

        private:
        Thread tickThread;    ///< The tick thread.
        Thread renderThread;  ///< The render thread.
    };
}  // namespace rome::core