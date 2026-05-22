#include "rm/app/strategy.hpp"

#include <gtest/gtest.h>

using namespace rome;
using namespace rome::core;

class StrategyProbe final : public ApplicationStrategy {
    public:
    StrategyProbe() : ApplicationStrategy([this](f64 dt) { observedTick = dt; }, [this](f64 dt) { observedRender = dt; }) {}

    void run(f64 tickRate, f64 renderRate) override {
        runs++;
        tick(1.0);
        render(2.0);
    }

    void finish() { status = Status::Done; }

    u32 runs = 0;
    f64 observedTick = 0.0;
    f64 observedRender = 0.0;
};

TEST(ApplicationStrategy, Start_WhenDone_RunsLoop) {
    StrategyProbe strategy;

    strategy.start(60, 120);

    EXPECT_EQ(strategy.runs, 1);
    EXPECT_DOUBLE_EQ(strategy.observedTick, 1.0);
    EXPECT_DOUBLE_EQ(strategy.observedRender, 2.0);
}

TEST(ApplicationStrategy, Start_WhenRunning_DoesNothing) {
    StrategyProbe strategy;

    strategy.start(60, 120);
    strategy.start(30, 30);

    EXPECT_EQ(strategy.runs, 1);

    strategy.finish();
}

TEST(ApplicationStrategy, PauseStop_Always_OK) {
    StrategyProbe strategy;

    strategy.start(60, 120);
    strategy.pause();
    strategy.stop();
    strategy.start(10, 20);

    EXPECT_EQ(strategy.runs, 2);
}
