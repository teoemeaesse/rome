#include "entry/application.h"

#include "chrono/timer.h"
#include "debug/log.h"
#include "debug/metrics.h"
#include "platform/platform.h"

namespace iodine::core {
    Application::Application(const Config& config)
        : config(config), status(Application::Status::Done), metrics({.framerateTarget = config.framerate}) {
        if (config.isMemoryLogging) {
            iodine::core::Metrics::getInstance().registerThread();
            iodine::core::Metrics::getInstance().setIsMemoryTracking(true);
        }
    }

    void Application::loop() {
        IO_INFO("Starting up game loop");

        f64 usPerFrame = 1000000 / metrics.framerateTarget, acc = 0, frametime = 0, counter = 0;
        u32 frames = 0;
        struct timespec prev, current;
        clock_gettime(CLOCK_MONOTONIC_RAW, &prev);
        while (status == Application::Status::Ok || status == Application::Status::Pause) {
            clock_gettime(CLOCK_MONOTONIC_RAW, &current);
            frametime = (current.tv_sec - prev.tv_sec) * 1000000.0f + (current.tv_nsec - prev.tv_nsec) / 1000.0f;
            prev = current;
            acc += frametime;

            while (acc >= usPerFrame) {
                if (status == Application::Status::Ok) {
                    tick();
                }
                acc -= usPerFrame;
            }

            frames++;
            counter += frametime;
            if (counter >= 1000000 * metrics.framerateWindow) {
                metrics.frameCount = frames;
                counter = 0;
                frames = 0;
            }

            render(frametime / 1000000.0f);

            if (Platform::getInstance().isSignal(Platform::Signal::INT)) {
                stop();
                IO_INFO("Caught SIGINT, stopping application");
                Platform::getInstance().clearSignal(Platform::Signal::INT);
            }
        }

        shutdown();
    }

    void Application::start() {
        if (status == Application::Status::Done) {
            status = Application::Status::Ok;
            IO_INFO("Starting application");
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