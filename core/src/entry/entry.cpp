#include "entry/entry.hpp"

#include <signal.h>

#include "debug/log.hpp"
#include "platform/platform.hpp"

iodine::i32 main(iodine::i32 argc, char** argv) {
    // Initialize the engine.
    iodine::core::Platform::getInstance().init();
    iodine::core::Platform::getInstance().assertCompatible();
    iodine::core::Platform::getInstance().log();
    iodine::core::Metrics::getInstance().start();

    // Create the application.
    auto app = createApplication();

    // Run the application.
    app->setup();
    app->start();

    // Cleanup.
    delete app;
    iodine::core::Metrics::getInstance().stop();

    return EXIT_SUCCESS;
}