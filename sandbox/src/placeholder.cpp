#include "entry/entry.h"

class MyApplication : public iodine::core::Application {
    public:
    MyApplication() : iodine::core::Application(iodine::core::Application::Builder().setTitle("My Application").enableMemoryLogging().build()) {}
    void setup() override {}
    void tick() override {}
    void render(iodine::f32 delta) override {}
};

iodine::core::Application* createApplication() { return new MyApplication(); }