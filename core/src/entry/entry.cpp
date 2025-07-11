#include "entry/entry.hpp"

#include <signal.h>

#include "debug/log.hpp"
#include "platform/platform.hpp"

rome::i32 main(rome::i32 argc, char** argv) {
    // Initialize the engine.
    rome::core::Platform::getInstance().init();
    rome::core::Platform::getInstance().assertCompatible();
    rome::core::Platform::getInstance().log();
    rome::core::Metrics::getInstance().start();

    // Create the application.
    auto app = createApplication();

    // Run the application.
    app->setup();
    app->start();

    // Cleanup.
    delete app;
    rome::core::Metrics::getInstance().stop();

    return EXIT_SUCCESS;
}