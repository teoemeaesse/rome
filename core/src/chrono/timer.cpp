#include "chrono/timer.h"

#include "platform/platform.h"

namespace iodine::core {
    void Timer::start() { lastTime = Platform::getInstance().getTime(); }

    f64 Timer::tick() {
        f64 currentTime = Platform::getInstance().getTime();
        f64 elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        return elapsedTime;
    }
}  // namespace iodine::core