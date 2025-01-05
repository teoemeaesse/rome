#include "debug/metrics.h"
#include "entry/entry.h"
#include "platform/platform.h"

using namespace iodine::core;

class MyApplication : public Application {
    public:
    MyApplication()
        : Application(Application::Builder().setTitle("My Application").enableMemoryLogging().setTickRate(30).setRenderRate(1000).build()) {}
    void setup() override {}
    void shutdown() override { Metrics::getInstance().report(); }
    void tick() override {
        IO_DEBUGV("Tick rate: %f | Framerate: %f", tickRate.getRate(), renderRate.getRate());
        if (Platform::getInstance().isSignal(Platform::Signal::INT)) {
            stop();
            IO_INFO("Caught SIGINT, stopping application");
            Platform::getInstance().clearSignal(Platform::Signal::INT);
        }
    }
    void render(iodine::f64 delta) override {}
};

iodine::core::Application* createApplication() { return new MyApplication(); }