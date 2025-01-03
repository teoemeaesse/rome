#pragma once

#include <signal.h>

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief Class representing the current platform.
     */
    class Platform {
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
         * @brief Performs platform initialization tasks such as setting up signal listeners.
         */
        Platform();
        virtual ~Platform() = default;

        /**
         * @brief Gets the singleton instance of the platform.
         * @return The platform instance.
         */
        static inline Platform getInstance() {
            static Platform instance;
            return instance;
        }

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
         * @brief Gets the current time in microseconds.
         * @return The current time.
         */
        u64 getTime();
    };
}  // namespace iodine::core