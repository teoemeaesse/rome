#include "platform/platform.hpp"

#ifdef IO_LINUX

#include <fcntl.h>
#include <unistd.h>

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
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = handleSigInt;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);

        struct sigaction sigTermHandler;
        sigTermHandler.sa_handler = handleSigTerm;
        sigemptyset(&sigTermHandler.sa_mask);
        sigTermHandler.sa_flags = 0;
        sigaction(SIGTERM, &sigTermHandler, NULL);

        struct sigaction sigAbrtHandler;
        sigAbrtHandler.sa_handler = handleSigAbrt;
        sigemptyset(&sigAbrtHandler.sa_mask);
        sigAbrtHandler.sa_flags = 0;
        sigaction(SIGABRT, &sigAbrtHandler, NULL);

        struct sigaction sigSegvHandler;
        sigSegvHandler.sa_handler = handleSigSegv;
        sigemptyset(&sigSegvHandler.sa_mask);
        sigSegvHandler.sa_flags = 0;
        sigaction(SIGSEGV, &sigSegvHandler, NULL);

        struct sigaction sigFpeHandler;
        sigFpeHandler.sa_handler = handleSigFpe;
        sigemptyset(&sigFpeHandler.sa_mask);
        sigFpeHandler.sa_flags = 0;
        sigaction(SIGFPE, &sigFpeHandler, NULL);

        struct sigaction sigIllHandler;
        sigIllHandler.sa_handler = handleSigIll;
        sigemptyset(&sigIllHandler.sa_mask);
        sigIllHandler.sa_flags = 0;
        sigaction(SIGILL, &sigIllHandler, NULL);

        struct sigaction sigHupHandler;
        sigHupHandler.sa_handler = handleSigHup;
        sigemptyset(&sigHupHandler.sa_mask);
        sigHupHandler.sa_flags = 0;
        sigaction(SIGHUP, &sigHupHandler, NULL);

        IO_INFO("Registered signal listeners");
    }

    const char* Platform::getName() { return PLATFORM_NAME; }

    void Platform::log() { IO_INFO("Running on %s", getName()); }

    void Platform::assertCompatible() {
        if (!isMacOS()) {
            IO_FATAL("Incompatible platform: %s", getName());
            std::exit(EXIT_FAILURE);
        }
    }

    b8 Platform::isWindows() { return false; }

    b8 Platform::isMacOS() { return true; }

    b8 Platform::isUnixLike() { return true; }

    b8 Platform::isLinux() { return false; }

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

    void Platform::clearSignal(Signal signal) {
        switch (signal) {
            case Signal::HUP:
                sigHup = 0;
                break;
            case Signal::INT:
                sigInt = 0;
                break;
            case Signal::ILL:
                sigIll = 0;
                break;
            case Signal::ABRT:
                sigAbrt = 0;
                break;
            case Signal::FPE:
                sigFpe = 0;
                break;
            case Signal::SEGV:
                sigSegv = 0;
                break;
            case Signal::TERM:
                sigTerm = 0;
                break;
            default:
                break;
        }
    }

    b8 Platform::isSignal(Signal signal) {
        switch (signal) {
            case Signal::HUP:
                return sigHup;
            case Signal::INT:
                return sigInt;
            case Signal::ILL:
                return sigIll;
            case Signal::ABRT:
                return sigAbrt;
            case Signal::FPE:
                return sigFpe;
            case Signal::SEGV:
                return sigSegv;
            case Signal::TERM:
                return sigTerm;
            default:
                return false;
        }
    }

    f64 Platform::time() { return std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count() / 1e6; }

    u64 Platform::timeNS() { return std::chrono::duration_cast<std::chrono::nanoseconds>(clock.now().time_since_epoch()).count(); }

    u64 Platform::randomU64() {
        static int fd = []() -> int {
            int fileDesc = open("/dev/urandom", O_RDONLY);
            if (fileDesc == -1) {
                exit(EXIT_FAILURE);
            }
            return fileDesc;
        }();

        u64 random_value;
        ssize_t bytes_read = read(fd, &random_value, sizeof(random_value));
        if (bytes_read != sizeof(random_value)) {
            exit(EXIT_FAILURE);
        }
        return random_value;
    }

    b8 Platform::isLittleEndian() {
        i8 i = 1;
        return *reinterpret_cast<byte*>(&i) == 1;
    }

    b8 Platform::isBigEndian() { return !isLittleEndian(); }

    void Platform::swapEndian(void* data, u64 size) {
        u8* bytes = reinterpret_cast<u8*>(data);
        for (u64 i = 0; i < size / 2; ++i) {
            u8 temp = bytes[i];
            bytes[i] = bytes[size - 1 - i];
            bytes[size - 1 - i] = temp;
        }
    }
}  // namespace iodine::core

#endif