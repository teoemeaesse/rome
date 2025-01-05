#include "prelude.h"

#include "debug/log.h"

void assertFail(const char* expression, const char* file, int line, const char* function) {
    if (!expression || !file || !function) IO_ERROR("\"assertFail\" called with NULL argument(s)");
    iodine::core::logMessage(iodine::core::LogLevel::Fatal, "Assertion failed: %s in file %s, line %d, function \"%s\"", expression, file, line,
                             function);
    abort();
}

void assertFail(const char* expression, const char* message, const char* file, int line, const char* function) {
    if (!expression || !message || !file || !function) IO_ERROR("\"assertFail\" called with NULL argument(s)");
    iodine::core::logMessage(iodine::core::LogLevel::Fatal, "Assertion failed: %s\n Message: \"%s\" in file %s, line %d, function \"%s\"", expression,
                             message, file, line, function);
    abort();
}