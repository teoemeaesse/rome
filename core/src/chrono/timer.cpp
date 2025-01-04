#include "chrono/timer.h"

namespace iodine::core {
    void Timer::start() { lastTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count(); }

    u64 Timer::tick() {
        u64 currentTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count();
        u64 elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        return elapsedTime;
    }
}  // namespace iodine::core