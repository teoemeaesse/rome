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
         * @brief Struct containing every uncleared signal received.
         */
        struct SignalStatus {
            enum At : u32 {
                None = 0,
                HUP = (1u << SIGHUP),
                INT = (1u << SIGINT),
                ILL = (1u << SIGILL),
                ABRT = (1u << SIGABRT),
                FPE = (1u << SIGFPE),
                SEGV = (1u << SIGSEGV),
                TERM = (1u << SIGTERM),
            };

            u32 bitmask;  ///< The bitmask of signals. Signals positioned at the bit index corresponding to their numeric value (i.e. SIGCHLD = 17).
        };

        static SignalStatus signalStatus;  ///< The current signal status.

        /**
         * @brief Performs platform initialization tasks such as setting up signal listeners.
         */
        static void init();

        /**
         * @brief Gets the name of the current platform.
         * @return The name of the current platform.
         */
        static const char* getName();

        /**
         * @brief Logs platform information.
         */
        static void log();

        /**
         * @brief Ensures the platform is compatible with iodine.
         */
        static void assertCompatible();

        /**
         * @brief Checks if the platform is Windows.
         * @return True if the platform is Windows, false otherwise.
         */
        static b8 isWindows();

        /**
         * @brief Checks if the platform is macos.
         * @return True if the platform is macos, false otherwise.
         */
        static b8 isMacOS();

        /**
         * @brief Checks if the platform is unix-like.
         * @return True if the platform is unix-like, false otherwise.
         */
        static b8 isUnixLike();

        /**
         * @brief Handles signals such as SIGINT.
         * @param signal The signal to handle.
         */
        static void handleSignal(i32 signal);

        /**
         * @brief Clears all signals, indicating they have been handled.
         */
        static inline void clearSignals() { signalStatus.bitmask &= SignalStatus::None; }
    };
}  // namespace iodine::core