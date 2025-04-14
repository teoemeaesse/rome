#pragma once

#include <signal.h>

#include "prelude.hpp"

namespace iodine::core {
    /**
     * @brief Class representing the current platform.
     */
    class IO_API Platform {
        public:
        /**
         * @brief Enum representing the low-level program signals.
         */
        enum Signal : u32 {
            None = 0,
            HUP = (1u << SIGHUP),
            INT = (1u << SIGINT),
            ILL = (1u << SIGILL),
            ABRT = (1u << SIGABRT),
            FPE = (1u << SIGFPE),
            SEGV = (1u << SIGSEGV),
            TERM = (1u << SIGTERM),
        };

        /**
         * @brief Gets the singleton instance of the platform.
         * @return The platform instance.
         */
        static inline Platform getInstance() {
            static Platform instance;
            return instance;
        }

        /**
         * @brief Performs platform initialization tasks such as setting up signal listeners.
         */
        void init();

        /**
         * @brief Gets the name of the current platform.
         * @return The name of the current platform.
         */
        const char* getName();

        /**
         * @brief Logs platform information.
         */
        void log();

        /**
         * @brief Ensures the platform is compatible with iodine.
         */
        void assertCompatible();

        /**
         * @brief Checks if the platform is Windows.
         * @return True if the platform is Windows, false otherwise.
         */
        b8 isWindows();

        /**
         * @brief Checks if the platform is macos.
         * @return True if the platform is macos, false otherwise.
         */
        b8 isMacOS();

        /**
         * @brief Checks if the platform is unix-like.
         * @return True if the platform is unix-like, false otherwise.
         */
        b8 isUnixLike();

        /**
         * @brief Checks if the platform is Linux.
         * @return True if the platform is Linux, false otherwise.
         */
        b8 isLinux();

        /**
         * @brief Clears a signal, marking it as handled.
         * @param signal The signal to clear.
         */
        void clearSignal(Signal signal);

        /**
         * @brief Checks if a signal has been received.
         * @param signal The signal to check.
         * @return True if the signal has been received, false otherwise.
         */
        b8 isSignal(Signal signal);

        /**
         * @brief Gets the current time in seconds.
         * @return The current time.
         */
        f64 time();

        /**
         * @brief Gets the current time in nanoseconds.
         * @return The current time in nanoseconds.
         */
        u64 timeNS();

        /**
         * @brief Generates a random 64-bit unsigned integer.
         * @return The random u64.
         */
        u64 randomU64();

        /**
         * @brief Checks if the platform is little-endian.
         * @return True if the platform is little-endian, false otherwise.
         */
        b8 isLittleEndian();

        /**
         * @brief Checks if the platform is big-endian.
         * @return True if the platform is big-endian, false otherwise.
         */
        b8 isBigEndian();

        /**
         * @brief Swaps the endianness of the given data.
         * @param data The data to swap.
         * @param size The size of the data.
         * @return The swapped data.
         */
        void swapEndian(void* data, u64 size);
    };
}  // namespace iodine::core