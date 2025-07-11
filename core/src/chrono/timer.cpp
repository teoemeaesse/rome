#include "chrono/timer.hpp"

#include "platform/platform.hpp"

namespace rome::core {
    void Timer::start() { lastTime = Platform::getInstance().time(); }

    f64 Timer::tick() {
        f64 currentTime = Platform::getInstance().time();
        f64 elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        return elapsedTime;
    }
}  // namespace rome::core