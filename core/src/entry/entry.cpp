#include "entry/entry.h"

#include <signal.h>

#include "debug/log.h"
#include "platform/platform.h"

iodine::i32 main(iodine::i32 argc, char** argv) {
    // Initialize the engine.
    iodine::core::Platform::getInstance().assertCompatible();
    iodine::core::Platform::getInstance().log();

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