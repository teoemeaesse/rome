#include "entry/entry.h"

#include <signal.h>

#include "debug/log.h"
#include "platform/platform.h"

iodine::i32 main(iodine::i32 argc, char** argv) {
    // Initialize the engine.
    iodine::core::Platform::getInstance().init();
    iodine::core::Platform::getInstance().assertCompatible();
    iodine::core::Platform::getInstance().log();

    // Create the application.
    auto app = createApplication();

    // Run the application.
    app->setup();
    app->start();

    // Cleanup.
    delete app;

    return EXIT_SUCCESS;
}