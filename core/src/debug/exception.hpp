#pragma once

#include "reflection/uuid.hpp"

namespace rome::core {
    class Exception : public std::exception {
        public:
        enum class Type {
            Assertion = 0,
            OutOfMemory,
            InvalidArgument,
            NotImplemented,
            NotFound,
            NotSupported,
        };

        /**
         * @brief Constructs a new Exception.
         * @param type The type of exception.
         * @param message The error message.
         * @param file The file where the error occurred.
         * @param line The line where the error occurred.
         * @param function The function where the error occurred.
         */
        Exception(Type type, const char* message, const char* file, u32 line, const char* function) noexcept;
        ~Exception() override = default;

        /**
         * @brief Gets the type of exception.
         * @return The type of exception.
         */
        Type getType() const noexcept;

        /**
         * @brief Follows up a prior exception with a new one.
         * @param type The type of exception.
         * @param message The error message.
         * @param file The file where the error occurred.
         * @param line The line where the error occurred.
         * @param function The function where the error occurred.
         */
        Exception& withFollowUp(Type type, const char* message, const char* file, u32 line, const char* function) noexcept;

        /**
         * @brief Gets the error message.
         * @return The error message.
         */
        const char* what() const noexcept override;

        private:
        struct Frame {
            Exception::Type type;  ///< The type of exception.
            std::string message;   ///< The error message.
            std::string file;      ///< The file where the error occurred.
            u32 line;              ///< The line where the error occurred.
            std::string function;  ///< The function where the error occurred.

            Frame(Type type, const char* message, const char* file, u32 line, const char* function) noexcept
                : type(type), message(message), file(file), line(line), function(function) {}
        };

        std::vector<Frame> frames;  ///< The stack of frames.

        /**
         * @brief Converts the exception type to a string.
         * @param type The type of exception.
         * @return The type as a string.
         */
        static const std::string& typeToString(Type type) noexcept;
    };
}  // namespace rome::core

/**
 * @brief Throws a core exception.
 * @param type The type of exception.
 * @param message The error message.
 */
#define THROW_CORE_EXCEPTION(type, message) throw rome::core::Exception(type, message, __FILE__, __LINE__, __FUNCTION__)

/**
 * @brief Follows up a prior exception with a new one.
 * @param exception The prior exception.
 * @param type The type of exception.
 * @param message The error message.
 */
#define BOUNCE_CORE_EXCEPTION(exception, type, message) throw exception.withFollowUp(exception, type, message, __FILE__, __LINE__, __FUNCTION__)