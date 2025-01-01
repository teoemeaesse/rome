#include "entry/entry.h"

#include <signal.h>

#include "debug/log.h"
#include "platform/platform.h"

namespace iodine::core {
    void handleCtrlC(i32 s) {
        IO_WARNV("Caught signal {0}", s);
        if (s == SIGINT) {
            IO_WARN("Shutting down");
            shutdownInterrupt = true;
        }
    }
}  // namespace iodine::core

iodine::i32 main(iodine::i32 argc, char** argv) {
    // Initialize the engine.
    iodine::core::Platform::assertCompatible();
    iodine::core::Platform::log();

    // Create the application.
    auto app = iodine::core::createApplication();

    // Handle interrupts.
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = iodine::core::handleCtrlC;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Run the application.
    app->setup();
    app->start();

    // Cleanup.
    delete app;

    return EXIT_SUCCESS;
}