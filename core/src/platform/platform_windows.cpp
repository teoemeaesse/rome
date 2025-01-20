#include "platform/platform.hpp"

#ifdef IO_WINDOWS

#include <chrono>

#include "debug/log.hpp"

namespace iodine::core {
    static volatile sig_atomic_t sigInt = 0;   ///< SIGINT signal.
    static volatile sig_atomic_t sigTerm = 0;  ///< SIGTERM signal.
    static volatile sig_atomic_t sigAbrt = 0;  ///< SIGABRT signal.
    static volatile sig_atomic_t sigSegv = 0;  ///< SIGSEGV signal.
    static volatile sig_atomic_t sigFpe = 0;   ///< SIGFPE signal.
    static volatile sig_atomic_t sigIll = 0;   ///< SIGILL signal.
    static volatile sig_atomic_t sigHup = 0;   ///< SIGHUP signal.
    static std::chrono::steady_clock clock;    ///< The current clock.

    void handleSigInt(i32 signal);
    void handleSigTerm(i32 signal);
    void handleSigAbrt(i32 signal);
    void handleSigSegv(i32 signal);
    void handleSigFpe(i32 signal);
    void handleSigIll(i32 signal);
    void handleSigHup(i32 signal);

    void Platform::init() {
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

    void handleSigInt(i32 signal) {
        if (signal == SIGINT) sigInt = 1;
    }

    void handleSigTerm(i32 signal) {
        if (signal == SIGTERM) sigTerm = 1;
    }

    void handleSigAbrt(i32 signal) {
        if (signal == SIGABRT) sigAbrt = 1;
    }

    void handleSigSegv(i32 signal) {
        if (signal == SIGSEGV) sigSegv = 1;
    }

    void handleSigFpe(i32 signal) {
        if (signal == SIGFPE) sigFpe = 1;
    }

    void handleSigIll(i32 signal) {
        if (signal == SIGILL) sigIll = 1;
    }

    void handleSigHup(i32 signal) {
        if (signal == SIGHUP) sigHup = 1;
    }
    switch (signal) {
        case SIGINT:
            IO_INFO("Caught signal SIGINT - raised by user on ctrl-c, alt-f4 or window close");
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

    void Platform::clearSignal(Signal signal) { signalState &= ~static_cast<u32>(signal); }

    b8 Platform::isSignal(Signal signal) { return signalState & static_cast<u32>(signal); }

    f64 Platform::time() { return std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count() / 1000000.0f; }
}  // namespace iodine::core

#endif