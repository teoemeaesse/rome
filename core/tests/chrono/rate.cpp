#include "rm/chrono/rate.hpp"

#include <gtest/gtest.h>

using namespace rome::core;

TEST(RateTracker, Constructors_Default_OK) {
    RateTracker tracker(60, 1.0);

    EXPECT_DOUBLE_EQ(tracker.getRate(), 0.0);
}

TEST(RateTracker, Tick_BelowWindow_DoesNotUpdateRate) {
    RateTracker tracker(60, 1.0);

    tracker.tick(0.25);
    tracker.tick(0.25);
    tracker.tick(0.25);

    EXPECT_DOUBLE_EQ(tracker.getRate(), 0.0);
}

TEST(RateTracker, Tick_OverWindow_UpdatesRate) {
    RateTracker tracker(60, 1.0);

    tracker.tick(0.5);
    EXPECT_DOUBLE_EQ(tracker.getRate(), 0.0);

    tracker.tick(0.6);
    EXPECT_GT(tracker.getRate(), 0.0);
}
