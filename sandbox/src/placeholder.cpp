#include "rm/app/twin_threads.hpp"
#include "rm/debug/metrics.hpp"
#include "rm/entry/entry.hpp"
#include "rm/platform/platform.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/external/string.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

struct MyReflectedStruct {
    int age;
    std::string name;

    RM_REFLECT;
};

RM_REFLECT_IMPL(MyReflectedStruct, "MyReflectedStruct", Fields().with("age", &MyReflectedStruct::age).with("name", &MyReflectedStruct::name));

class MyApplication : public Application {
    public:
    MyApplication()
        : Application(Application::Builder().setTitle("My Application").enableMemoryLogging().setTickRate(30).setRenderRate(1000).build()) {}
    void setup() override {}
    void shutdown() override { Metrics::getInstance().report(); }
    void tick(f64 dt) override {
        tickRate.tick(dt);
        RM_DEBUG("Tick rate: %.2f | Framerate: %.2f", tickRate.getRate(), renderRate.getRate());
        UUID uuid;
        RM_DEBUG("UUID: %s", uuid.toString().c_str());
        if (Platform::getInstance().isSignal(Platform::Signal::INT)) {
            shutdown();
            stop();
            RM_INFO("Caught SIGINT, stopping application");
            Platform::getInstance().clearSignal(Platform::Signal::INT);
        }
    }
    void render(f64 dt) override { renderRate.tick(dt); }
};

Application* createApplication() { return new MyApplication(); }
