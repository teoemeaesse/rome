#include "debug/exception.hpp"

#include "debug/log.hpp"

namespace rome::core {
    const std::string& Exception::typeToString(Type type) noexcept {
        static const std::string typeStrings[] = {
            "Assertion", "OutOfMemory", "InvalidArgument", "NotImplemented", "NotFound", "NotSupported",
        };
        return typeStrings[static_cast<int>(type)];
    }

    Exception::Exception(Type type, const char* message, const char* file, u32 line, const char* function) noexcept : frames() {
        frames.emplace_back(type, message, file, line, function);
        updateMessage();
    }

    Exception::Type Exception::getType() const noexcept { return frames.at(frames.size() - 1).type; }

    Exception& Exception::withFollowUp(Type type, const char* message, const char* file, u32 line, const char* function) noexcept {
        frames.emplace_back(type, message, file, line, function);
        updateMessage();
        return *this;
    }

    const char* Exception::what() const noexcept { return message.c_str(); }

    void Exception::updateMessage() {
        message = "";
        for (u64 i = 0; i < frames.size(); i++) {
            message += "#" + std::to_string(i) + " - " + typeToString(frames.at(i).type) + ": \"" + frames.at(i).message + "\"\n\t" +
                       frames.at(i).file + ":" + std::to_string(frames.at(i).line);
        }
    }
}  // namespace rome::core
