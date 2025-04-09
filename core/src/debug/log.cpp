#include "debug/log.hpp"

#include <cstdarg>
#include <iostream>

namespace iodine::core {
    namespace ANSIColors {
        constexpr const char* Black = "\x1b[38;5;0m";
        constexpr const char* Gray = "\x1b[38;5;7m";
        constexpr const char* Red = "\x1b[38;5;196m";
        constexpr const char* Orange = "\x1b[38;5;208m";
        constexpr const char* Green = "\x1b[38;5;46m";
        constexpr const char* Yellow = "\x1b[38;5;226m";
        constexpr const char* Blue = "\x1b[38;5;33m";
        constexpr const char* Magenta = "\x1b[38;5;201m";
        constexpr const char* Cyan = "\x1b[38;5;123m";
        constexpr const char* White = "\x1b[38;5;15m";
        constexpr const char* Default = "\x1b[39m";
    }  // namespace ANSIColors

    void logMessage(LogLevel level, const char* message, ...) {
        std::string level_strings[6] = {std::string(ANSIColors::White) + "[TRACE]: " + ANSIColors::Default, std::string(ANSIColors::Gray) + "[DEBUG]: " + ANSIColors::Default,
                                        std::string(ANSIColors::Blue) + "[INFO]:  " + ANSIColors::Default,  std::string(ANSIColors::Orange) + "[WARN]:  " + ANSIColors::Default,
                                        std::string(ANSIColors::Red) + "[ERROR]: " + ANSIColors::Default,   std::string(ANSIColors::Magenta) + "[FATAL]: " + ANSIColors::Default};

        const std::string& level_string = level_strings[static_cast<u8>(level)];
        std::cout << level_string;

        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);
        std::cout << std::endl;
    }
}  // namespace iodine::core
