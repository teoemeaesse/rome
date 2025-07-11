#include "app/twin_threads.hpp"

#include "app/app.hpp"
#include "chrono/timer.hpp"
#include "debug/exception.hpp"

namespace rome::core {
    TwinStrategy::TwinStrategy(Application& app, b8 memoryMetrics)
        : ApplicationStrategy([this, &app](f64 dt) { app.tick(dt); }, [this, &app](f64 dt) { app.render(dt); }),
          tickThread("Tick"),
          renderThread("Render") {
        this->memoryMetrics = memoryMetrics;
    }

    void TwinStrategy::run(f64 tickRate, f64 renderRate) {
        tickThread.run([this, &tickRate]() {
            RM_INFO("Starting up tick thread");

            if (memoryMetrics) {
                rome::core::Metrics::getInstance().registerThread("Tick");
                rome::core::Metrics::getInstance().setIsMemoryTracking(true);
                RM_INFO("Metrics tracking ON for tick thread ID: %s", tickThread.getID().toString().c_str());
            }

            f64 targetTime = 1.0 / tickRate;
            f64 elapsed = 0.0;
            Timer loopTimer;
            loopTimer.start();
            while (status == Status::Ok || status == Status::Pause) {
                elapsed += loopTimer.tick();

                try {
                    if (status == Status::Ok) {
                        while (elapsed >= targetTime) {
                            this->tick(elapsed);
                            elapsed -= targetTime;
                        }
                    }
                } catch (const Exception& e) {
                    RM_ERROR(e.what());
                }
            }
        });

        renderThread.run([this, &renderRate]() {
            RM_INFO("Starting up render thread");

            if (memoryMetrics) {
                rome::core::Metrics::getInstance().registerThread("Render");
                rome::core::Metrics::getInstance().setIsMemoryTracking(true);
                RM_INFO("Metrics tracking ON for render thread ID: %s", renderThread.getID().toString().c_str());
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
        RM_INFO("Tick thread finished");
        renderThread.join();
        RM_INFO("Render thread finished");
    }
}  // namespace rome::core