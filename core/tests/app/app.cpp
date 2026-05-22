#include "rm/app/app.hpp"

#include <gtest/gtest.h>

using namespace rome;
using namespace rome::core;

class TestStrategy final : public ApplicationStrategy {
    public:
    TestStrategy() : ApplicationStrategy([](f64) {}, [](f64) {}) {}

    void run(f64 tickRate, f64 renderRate) override {
        observedTickRate = tickRate;
        observedRenderRate = renderRate;
        runs++;
        status = Status::Done;
    }

    u32 runs = 0;
    f64 observedTickRate = 0.0;
    f64 observedRenderRate = 0.0;
};

class TestApplication final : public Application {
    public:
    TestApplication(const Config& config, Unique<ApplicationStrategy>&& strategy) : Application(config, std::move(strategy)) {}

    void setup() override {}
    void shutdown() override {}
    void tick(f64) override { ticks++; }
    void render(f64) override { renders++; }

    const Config& getConfig() const { return config; }

    u32 ticks = 0;
    u32 renders = 0;
};

TEST(ApplicationBuilder, Build_CustomValues_OK) {
    Application::Config config = Application::Builder()
                                     .setTitle("Test App")
                                     .setTickRate(30)
                                     .setTickRateWindow(2.0)
                                     .setRenderRate(120)
                                     .setRenderRateWindow(0.5)
                                     .enableMemoryLogging()
                                     .enablePerformanceLogging()
                                     .build();

    EXPECT_EQ(config.title, "Test App");
    EXPECT_EQ(config.tickRate, 30);
    EXPECT_DOUBLE_EQ(config.tickRateWindow, 2.0);
    EXPECT_EQ(config.renderRate, 120);
    EXPECT_DOUBLE_EQ(config.renderRateWindow, 0.5);
    EXPECT_TRUE(config.isMemoryLogging);
    EXPECT_TRUE(config.isPerformanceLogging);
}

TEST(Application, Start_CustomStrategy_OK) {
    auto* rawStrategy = new TestStrategy();
    TestApplication app(Application::Builder().setTickRate(15).setRenderRate(45).build(), Unique<ApplicationStrategy>(rawStrategy));

    app.start();

    EXPECT_EQ(rawStrategy->runs, 1);
    EXPECT_DOUBLE_EQ(rawStrategy->observedTickRate, 15.0);
    EXPECT_DOUBLE_EQ(rawStrategy->observedRenderRate, 45.0);
}
