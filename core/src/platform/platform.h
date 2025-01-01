#pragma once

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief Class representing the current platform.
     */
    class Platform {
        public:
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
    };
}  // namespace iodine::core