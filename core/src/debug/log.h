#pragma once

#include <string>

#include "prelude.h"

namespace iodine::core {
    /**
     * @brief Log types in ascending order of importance.
     */
    enum class LogLevel : u8 { Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Fatal = 5 };

    /**
     * @brief Writes out a log message to stdout.
     * @param level The log level of the message.
     * @param message The message to log.
     * @param ... The arguments to format the message with.
     */
    void logMessage(LogLevel level, const char*, ...);
}  // namespace iodine::core

/* Mute debug logs in release build */
#ifdef IO_DEBUG_ON
#define IO_LOG_TRACE_ON
#define IO_LOG_DEBUG_ON
#endif

/* Enable trace logging */
#ifdef IO_LOG_TRACE_ON
/**
 * @brief Logs a trace message.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define IO_TRACEV(message, ...) iodine::core::logMessage(iodine::core::LogLevel::Trace, message, ##__VA_ARGS__)
#define IO_TRACE(message) iodine::core::logMessage(iodine::core::LogLevel::Trace, message)
#else
#define IO_TRACEV(message, ...)
#define IO_TRACE(message)
#endif

/* Enable debug logging */
#ifdef IO_LOG_DEBUG_ON
/**
 * @brief Logs a debug message.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define IO_DEBUGV(message, ...) iodine::core::logMessage(iodine::core::LogLevel::Debug, message, ##__VA_ARGS__)
#define IO_DEBUG(message) iodine::core::logMessage(iodine::core::LogLevel::Debug, message)
#else
#define IO_DEBUGV(message, ...)
#define IO_DEBUG(message)
#endif

/* Enable info logging */
/**
 * @brief Logs an info message.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define IO_INFOV(message, ...) iodine::core::logMessage(iodine::core::LogLevel::Info, message, ##__VA_ARGS__)
#define IO_INFO(message) iodine::core::logMessage(iodine::core::LogLevel::Info, message)

/* Enable warn logging */
/**
 * @brief Logs a warning.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define IO_WARNV(message, ...) iodine::core::logMessage(iodine::core::LogLevel::Warn, message, ##__VA_ARGS__)
#define IO_WARN(message) iodine::core::logMessage(iodine::core::LogLevel::Warn, message)

/* Always enable error and fatal logging */
/**
 * @brief Logs an error.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define IO_ERRORV(message, ...) iodine::core::logMessage(iodine::core::LogLevel::Error, message, ##__VA_ARGS__)
#define IO_ERROR(message) iodine::core::logMessage(iodine::core::LogLevel::Error, message)

/**
 * @brief Logs a fatal error.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define IO_FATALV(message, ...) iodine::core::logMessage(iodine::core::LogLevel::Fatal, message, ##__VA_ARGS__)
#define IO_FATAL(message) iodine::core::logMessage(iodine::core::LogLevel::Fatal, message)