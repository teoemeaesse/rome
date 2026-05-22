#include "rm/chrono/timer.hpp"

#include <gtest/gtest.h>

using namespace rome::core;

TEST(Timer, Tick_Once_ReturnsAnElapsedTime) {
    Timer timer;

    timer.start();
    rome::f64 elapsed = timer.tick();

    EXPECT_GE(elapsed, 0.0);
}

TEST(Timer, Tick_Repeated_ReturnsAnElapsedTime) {
    Timer timer;

    timer.start();
    timer.tick();
    rome::f64 elapsed = timer.tick();

    EXPECT_GE(elapsed, 0.0);
}
