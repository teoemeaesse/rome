#include "app/twin_threads.hpp"

#include "app/app.hpp"
#include "chrono/timer.hpp"

namespace iodine::core {
    TwinStrategy::TwinStrategy(Application& app, b8 memoryMetrics)
        : ApplicationStrategy([this, &app](f64 dt) { app.tick(dt); }, [this, &app](f64 dt) { app.render(dt); }) {
        this->memoryMetrics = memoryMetrics;
    }

    void TwinStrategy::run(f64 tickRate, f64 renderRate) {
        tickThread.run([this, &tickRate]() {
            IO_INFO("Starting up tick thread");

            if (memoryMetrics) {
                iodine::core::Metrics::getInstance().registerThread();
                iodine::core::Metrics::getInstance().setIsMemoryTracking(true);
                IO_INFO("Metrics tracking ON for tick thread ID: %d", tickThread.getThreadId());
            }

            f64 targetTime = 1.0 / tickRate;
            f64 elapsed = 0.0;
            Timer loopTimer;
            loopTimer.start();
            while (status == Status::Ok || status == Status::Pause) {
                elapsed += loopTimer.tick();

                if (status == Status::Ok) {
                    while (elapsed >= targetTime) {
                        this->tick(elapsed);
                        elapsed -= targetTime;
                    }
                }
            }
        });

        renderThread.run([this, &renderRate]() {
            IO_INFO("Starting up render thread");

            if (memoryMetrics) {
                iodine::core::Metrics::getInstance().registerThread();
                iodine::core::Metrics::getInstance().setIsMemoryTracking(true);
                IO_INFO("Metrics tracking ON for tick thread ID: %d", renderThread.getThreadId());
            }

            f64 targetTime = 1.0 / renderRate;
            f64 elapsed = 0.0;
            Timer loopTimer;
            loopTimer.start();
            while (status == Status::Ok || status == Status::Pause) {
                elapsed += loopTimer.tick();

                if (elapsed >= targetTime) {
                    this->render(elapsed);
                    elapsed -= targetTime;
                }
            }
        });

        tickThread.join();
        renderThread.join();
    }
}  // namespace iodine::core