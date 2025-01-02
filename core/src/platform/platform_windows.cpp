#include "platform/platform.h"

#ifdef IO_WINDOWS

namespace iodine::core {
    void Platform::init() {
        // Set up signal listeners
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
        signal(SIGABRT, handleSignal);
        signal(SIGSEGV, handleSignal);
        signal(SIGFPE, handleSignal);
        signal(SIGILL, handleSignal);
        signal(SIGHUP, handleSignal);
    }

    const char* Platform::getName() { return PLATFORM_NAME; }

    void Platform::log() { IO_INFOV("Running on {0}", getName()); }

    void Platform::assertCompatible() {
        if (!isWindows()) {
            IO_FATALV("Incompatible platform: {0}", getName());
            std::exit(EXIT_FAILURE);
        }
    }

    b8 Platform::isWindows() { return true; }

    b8 Platform::isMacOS() { return false; }

    b8 Platform::isUnixLike() { return false; }

    void Platform::handleSignal(i32 signal) {
        switch (signal) {
            case SIGINT:
                IO_INFO("Caught signal SIGINT - raised by user on ctrl-c, alt-f4 or window close");
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