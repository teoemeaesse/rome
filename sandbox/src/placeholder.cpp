#include "debug/metrics.h"
#include "entry/entry.h"

class MyApplication : public iodine::core::Application {
    public:
    MyApplication()
        : iodine::core::Application(iodine::core::Application::Builder().setTitle("My Application").enableMemoryLogging().setFramerate(1).build()) {}
    void setup() override {}
    void shutdown() override { iodine::core::Metrics::getInstance().report(); }
    void tick() override { IO_INFO("Hello, world!"); }
    void render(iodine::f32 delta) override {}
};

iodine::core::Application* createApplication() { return new MyApplication(); }