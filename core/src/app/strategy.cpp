#include "app/strategy.hpp"

#include "app/app.hpp"
#include "debug/log.hpp"

namespace iodine::core {
    ApplicationStrategy::ApplicationStrategy(const std::function<void(f64)>& tick, const std::function<void(f64)>& render)
        : tick(tick), render(render), memoryMetrics(false) {}

    void ApplicationStrategy::start(f64 tickRate, f64 renderRate) {
        if (status == Status::Done) {
            status = Status::Ok;
            IO_INFO("Starting application");
            run(tickRate, renderRate);
        } else if (status == Status::Pause) {
            status = Status::Ok;
        } else {
            IO_WARN("Application already running");
        }
    }

    void ApplicationStrategy::pause() {
        if (status == Status::Ok) {
            status = Status::Pause;
        } else {
            IO_WARN("Application is not running");
        }
    }

    void ApplicationStrategy::stop() {
        if (status == Status::Ok || status == Status::Pause) {
            status = Status::Done;
        } else {
            IO_WARN("Application is not running");
        }
    }
}  // namespace iodine::core