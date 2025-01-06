#include "entry/application.h"

#include "chrono/timer.h"
#include "debug/log.h"
#include "debug/metrics.h"
#include "platform/platform.h"

namespace iodine::core {
    Application::Application(const Config& config)
        : config(config),
          status(Application::Status::Done),
          tickRate(config.tickRate, config.tickRateWindow),
          renderRate(config.renderRate, config.renderRateWindow) {
        if (config.isMemoryLogging) {
            iodine::core::Metrics::getInstance().registerThread();
            iodine::core::Metrics::getInstance().setIsMemoryTracking(true);
        }
    }

    void Application::loop() {
        IO_INFO("Starting up game loop");

        f64 targetTickTime = 1.0f / config.tickRate;
        f64 targetRenderTime = 1.0 / config.renderRate;
        f64 targetLoopTime = targetTickTime + targetRenderTime;
        f64 elapsedTick, elapsedRender, loopTime;
        Timer loopTimer;
        loopTimer.start();
        while (status == Application::Status::Ok || status == Application::Status::Pause) {
            loopTime = loopTimer.tick();
            elapsedTick += loopTime;
            elapsedRender += loopTime;

            while (elapsedTick >= targetTickTime) {
                if (status == Application::Status::Ok) {
                    tickRate.tick(elapsedTick);
                    tick();
                }
                elapsedTick -= targetTickTime;
            }

            if (elapsedRender >= targetRenderTime) {
                renderRate.tick(elapsedRender);
                render(loopTime);
                elapsedRender -= targetRenderTime;
            }
        }

        shutdown();
    }

    void Application::start() {
        if (status == Application::Status::Done) {
            status = Application::Status::Ok;
            IO_INFO("Starting application");
            loop();
        } else if (status == Application::Status::Pause) {
            status = Application::Status::Ok;
        } else {
            IO_WARN("Application already running");
        }
    }

    void Application::stop() {
        if (status == Application::Status::Ok || status == Application::Status::Pause) {
            status = Application::Status::Done;
        } else {
            IO_WARN("Application is not running");
        }
    }
}  // namespace iodine::core