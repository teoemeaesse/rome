#pragma once

#include "prelude.hpp"

namespace rome::core {
    /**
     * @brief Log types in ascending order of importance.
     */
    enum class RM_API LogLevel : u8 { Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Fatal = 5 };

    /**
     * @brief Writes out a log message to stdout.
     * @param level The log level of the message.
     * @param message The message to log.
     * @param ... The arguments to format the message with.
     */
    void RM_API logMessage(LogLevel level, const char*, ...);
}  // namespace rome::core

/* Mute debug logs in release build */
#ifdef RM_DEBUG_ON
#define RM_LOG_TRACE_ON
#define RM_LOG_DEBUG_ON
#endif

/* Enable trace logging */
#ifdef RM_LOG_TRACE_ON
/**
 * @brief Logs a trace message.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define RM_TRACE(message, ...) rome::core::logMessage(rome::core::LogLevel::Trace, message __VA_OPT__(, ##__VA_ARGS__))
#else
#define RM_TRACE(message, ...)
#endif

/* Enable debug logging */
#ifdef RM_LOG_DEBUG_ON
/**
 * @brief Logs a debug message.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define RM_DEBUG(message, ...) rome::core::logMessage(rome::core::LogLevel::Debug, message __VA_OPT__(, ##__VA_ARGS__))
#else
#define RM_DEBUG(message, ...)
#endif

/* Enable info logging */
/**
 * @brief Logs an info message.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define RM_INFO(message, ...) rome::core::logMessage(rome::core::LogLevel::Info, message __VA_OPT__(, ##__VA_ARGS__))

/* Enable warn logging */
/**
 * @brief Logs a warning.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define RM_WARN(message, ...) rome::core::logMessage(rome::core::LogLevel::Warn, message __VA_OPT__(, ##__VA_ARGS__))

/* Always enable error and fatal logging */
/**
 * @brief Logs an error.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define RM_ERROR(message, ...) rome::core::logMessage(rome::core::LogLevel::Error, message __VA_OPT__(, ##__VA_ARGS__))

/**
 * @brief Logs a fatal error.
 * @param message The message to log.
 * @param ... Log message format arguments.
 */
#define RM_FATAL(message, ...) rome::core::logMessage(rome::core::LogLevel::Fatal, message __VA_OPT__(, ##__VA_ARGS__))