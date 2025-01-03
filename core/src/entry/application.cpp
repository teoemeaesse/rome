#include "entry/application.h"

#include "debug/log.h"
#include "debug/metrics.h"
#include "platform/platform.h"

namespace iodine::core {
    Application::Application(const Config& config)
        : config(config), status(Application::Status::Done), metrics({.framerateTarget = config.framerate}) {
        if (config.isMemoryLogging) {
            iodine::core::Metrics::getInstance().enableMemory();
        }
    }

    void Application::loop() {
        IO_INFO("Starting up game loop");

        u64 delta = 1000000 / metrics.framerateTarget, acc = 0, frametime = 0, counter = 0;
        u32 frames = 0;
        struct timespec start, end;
        while (status == Application::Status::Ok || status == Application::Status::Pause) {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            while (acc >= delta) {
                if (status == Application::Status::Ok) {
                    tick();
                }
                acc -= delta;
            }

            frames++;
            counter += frametime;
            if (counter >= 1000000 * metrics.framerateWindow) {
                metrics.frameCount = frames;
                counter = 0;
                frames = 0;
            }
            f32 delta = frametime / 1000000.0f;

            render(delta);

            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
            frametime = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
            acc += frametime;

            if (Platform::getInstance().isSignal(Platform::Signal::INT)) {
                stop();
                Platform::getInstance().clearSignal(Platform::Signal::INT);
            }
        }
    }

    void Application::start() {
        if (status == Application::Status::Done) {
            status = Application::Status::Ok;
            loop();
        } else if (status == Application::Status::Pause) {
            status = Application::Status::Ok;
        } else {
            IO_WARN("Application already running");
        }
    }

    void Application::stop() {
        if (status == Application::Status::Ok || status == Application::Status::Pause) {
            status = Application::Status::Done;
        } else {
            IO_WARN("Application is not running");
        }
    }
}  // namespace iodine::core