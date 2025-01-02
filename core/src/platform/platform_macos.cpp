#include <signal.h>

#include "debug/log.h"
#include "platform/platform.h"

#ifdef IO_MACOS

namespace iodine::core {
    void Platform::init() {
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);

        struct sigaction sigTermHandler;
        sigTermHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigTermHandler.sa_mask);
        sigTermHandler.sa_flags = 0;
        sigaction(SIGTERM, &sigTermHandler, NULL);

        struct sigaction sigAbrtHandler;
        sigAbrtHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigAbrtHandler.sa_mask);
        sigAbrtHandler.sa_flags = 0;
        sigaction(SIGABRT, &sigAbrtHandler, NULL);

        struct sigaction sigSegvHandler;
        sigSegvHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigSegvHandler.sa_mask);
        sigSegvHandler.sa_flags = 0;
        sigaction(SIGSEGV, &sigSegvHandler, NULL);

        struct sigaction sigFpeHandler;
        sigFpeHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigFpeHandler.sa_mask);
        sigFpeHandler.sa_flags = 0;
        sigaction(SIGFPE, &sigFpeHandler, NULL);

        struct sigaction sigIllHandler;
        sigIllHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigIllHandler.sa_mask);
        sigIllHandler.sa_flags = 0;
        sigaction(SIGILL, &sigIllHandler, NULL);

        struct sigaction sigHupHandler;
        sigHupHandler.sa_handler = iodine::core::Platform::handleSignal;
        sigemptyset(&sigHupHandler.sa_mask);
        sigHupHandler.sa_flags = 0;
        sigaction(SIGHUP, &sigHupHandler, NULL);

        IO_INFO("Registered signal listeners");
    }

    const char* Platform::getName() { return PLATFORM_NAME; }

    void Platform::log() { IO_INFOV("Running on {0}", getName()); }

    void Platform::assertCompatible() {
        if (!isMacOS()) {
            IO_FATALV("Incompatible platform: {0}", getName());
            std::exit(EXIT_FAILURE);
        }
    }

    b8 Platform::isWindows() { return false; }

    b8 Platform::isMacOS() { return true; }

    b8 Platform::isUnixLike() { return true; }

    void Platform::handleSignal(i32 signal) {
        switch (signal) {
            case SIGINT:
                IO_INFO("Caught signal SIGINT - raised by user on ctrl-c, cmd-q or window close");
                signalStatus.bitmask |= SignalStatus::INT;
                break;
            case SIGTERM:
                IO_INFO("Caught signal SIGTERM - raised by an external process to terminate the program");
                signalStatus.bitmask |= SignalStatus::TERM;
                break;
            case SIGABRT:
                IO_INFO("Caught signal SIGABRT - assertion failed");
                signalStatus.bitmask |= SignalStatus::ABRT;
                break;
            case SIGSEGV:
                IO_INFO("Caught signal SIGSEGV - segmentation fault detected, dumping memory log");  // TODO: Implement memory tracking
                signalStatus.bitmask |= SignalStatus::SEGV;
                break;
            case SIGFPE:
                IO_INFO("Caught signal SIGFPE - invalid math operation");
                signalStatus.bitmask |= SignalStatus::FPE;
                break;
            case SIGILL:
                IO_INFO("Caught signal SIGILL - illegal instruction");
                signalStatus.bitmask |= SignalStatus::ILL;
                break;
            case SIGHUP:
                IO_INFO("Caught signal SIGHUP - hangup detected, reloading engine");  // TODO: Implement hot reloading (plugins etc.)
                signalStatus.bitmask |= SignalStatus::HUP;
                break;
            default:
                IO_WARNV("Caught unhandled signal {0}", signal);
                break;
        }
    }
}  // namespace iodine::core

#endif