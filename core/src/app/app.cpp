#include "app/app.h"

#include "app/twin_threads.h"

namespace iodine::core {
    Application::Application(const Config& config)
        : config(config),
          strategy(std::move(MakeUnique<TwinStrategy>(*this))),
          tickRate(config.tickRate, config.tickRateWindow),
          renderRate(config.renderRate, config.renderRateWindow) {
        iodine::core::Metrics::getInstance().registerThread();
        if (config.isMemoryLogging) {
            iodine::core::Metrics::getInstance().setIsMemoryTracking(true);
        }
    }

    Application::Application(const Config& config, Unique<ApplicationStrategy>&& strategy)
        : config(config),
          strategy(std::move(strategy)),
          tickRate(config.tickRate, config.tickRateWindow),
          renderRate(config.renderRate, config.renderRateWindow) {
        iodine::core::Metrics::getInstance().registerThread();
        if (config.isMemoryLogging) {
            iodine::core::Metrics::getInstance().setIsMemoryTracking(true);
        }
    }

    void Application::start() { strategy->start(config.tickRate, config.renderRate); }

    void Application::pause() { strategy->pause(); }

    void Application::stop() { strategy->stop(); }
}  // namespace iodine::core