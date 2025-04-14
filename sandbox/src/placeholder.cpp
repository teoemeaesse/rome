#include "app/twin_threads.hpp"
#include "debug/metrics.hpp"
#include "entry/entry.hpp"
#include "platform/platform.hpp"
#include "reflection/external/primitives.hpp"
#include "reflection/external/string.hpp"
#include "reflection/traits/field.hpp"

using namespace iodine;
using namespace iodine::core;

struct MyReflectedStruct {
    int age;
    std::string name;

    IO_REFLECT;
};

IO_REFLECT_IMPL(MyReflectedStruct, "MyReflectedStruct", Fields().with("age", &MyReflectedStruct::age).with("name", &MyReflectedStruct::name));

class MyApplication : public Application {
    public:
    MyApplication() : Application(Application::Builder().setTitle("My Application").enableMemoryLogging().setTickRate(30).setRenderRate(1000).build()) {}
    void setup() override {}
    void shutdown() override { Metrics::getInstance().report(); }
    void tick(f64 dt) override {
        tickRate.tick(dt);
        IO_DEBUG("Tick rate: %.2f | Framerate: %.2f", tickRate.getRate(), renderRate.getRate());
        UUID uuid;
        IO_DEBUG("UUID: %s", uuid.toString().c_str());
        if (Platform::getInstance().isSignal(Platform::Signal::INT)) {
            shutdown();
            stop();
            IO_INFO("Caught SIGINT, stopping application");
            Platform::getInstance().clearSignal(Platform::Signal::INT);
        }
    }
    void render(f64 dt) override { renderRate.tick(dt); }
};

Application* createApplication() { return new MyApplication(); }