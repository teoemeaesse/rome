#include "platform/platform.h"

#ifdef IO_MACOS

#include <signal.h>

#include "debug/log.h"

namespace iodine::core {
    static u32 signalState;                  ///< The current signal status.
    static std::chrono::steady_clock clock;  ///< The current clock.

    static void handleSignal(i32 signal);

    Platform::Platform() {
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = handleSignal;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);

        struct sigaction sigTermHandler;
        sigTermHandler.sa_handler = handleSignal;
        sigemptyset(&sigTermHandler.sa_mask);
        sigTermHandler.sa_flags = 0;
        sigaction(SIGTERM, &sigTermHandler, NULL);

        struct sigaction sigAbrtHandler;
        sigAbrtHandler.sa_handler = handleSignal;
        sigemptyset(&sigAbrtHandler.sa_mask);
        sigAbrtHandler.sa_flags = 0;
        sigaction(SIGABRT, &sigAbrtHandler, NULL);

        struct sigaction sigSegvHandler;
        sigSegvHandler.sa_handler = handleSignal;
        sigemptyset(&sigSegvHandler.sa_mask);
        sigSegvHandler.sa_flags = 0;
        sigaction(SIGSEGV, &sigSegvHandler, NULL);

        struct sigaction sigFpeHandler;
        sigFpeHandler.sa_handler = handleSignal;
        sigemptyset(&sigFpeHandler.sa_mask);
        sigFpeHandler.sa_flags = 0;
        sigaction(SIGFPE, &sigFpeHandler, NULL);

        struct sigaction sigIllHandler;
        sigIllHandler.sa_handler = handleSignal;
        sigemptyset(&sigIllHandler.sa_mask);
        sigIllHandler.sa_flags = 0;
        sigaction(SIGILL, &sigIllHandler, NULL);

        struct sigaction sigHupHandler;
        sigHupHandler.sa_handler = handleSignal;
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

    static void handleSignal(i32 signal) {
        switch (signal) {
            case SIGINT:
                IO_INFO("Caught signal SIGINT - raised by user on ctrl-c, cmd-q or window close");
                signalState |= Platform::Signal::INT;
                break;
            case SIGTERM:
                IO_INFO("Caught signal SIGTERM - raised by an external process to terminate the program");
                signalState |= Platform::Signal::TERM;
                break;
            case SIGABRT:
                IO_INFO("Caught signal SIGABRT - assertion failed");
                signalState |= Platform::Signal::ABRT;
                break;
            case SIGSEGV:
                IO_INFO("Caught signal SIGSEGV - segmentation fault detected, dumping memory log");  // TODO: Implement memory tracking
                signalState |= Platform::Signal::SEGV;
                break;
            case SIGFPE:
                IO_INFO("Caught signal SIGFPE - invalid math operation");
                signalState |= Platform::Signal::FPE;
                break;
            case SIGILL:
                IO_INFO("Caught signal SIGILL - illegal instruction");
                signalState |= Platform::Signal::ILL;
                break;
            case SIGHUP:
                IO_INFO("Caught signal SIGHUP - hangup detected, reloading engine");  // TODO: Implement hot reloading (plugins etc.)
                signalState |= Platform::Signal::HUP;
                break;
            default:
                IO_WARNV("Caught unhandled signal {0}", signal);
                break;
        }
    }

    void Platform::clearSignal(Signal signal) { signalState &= ~static_cast<u32>(signal); }

    b8 Platform::isSignal(Signal signal) { return signalState & static_cast<u32>(signal); }

    u64 Platform::getTime() { return std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count(); }
}  // namespace iodine::core

#endif