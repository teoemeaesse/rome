#include "debug/exception.hpp"

namespace iodine::core {
    const std::string& Exception::typeToString(Type type) noexcept {
        static const std::string typeStrings[] = {
            "Assertion", "OutOfMemory", "InvalidArgument", "NotImplemented", "NotFound", "NotSupported",
        };
        return typeStrings[static_cast<int>(type)];
    }

    Exception::Exception(Type type, const char* message, const char* file, u32 line, const char* function) noexcept : frames() {
        frames.emplace_back(type, message, file, line, function);
    }

    Exception::Type Exception::getType() const noexcept { return frames.at(frames.size() - 1).type; }

    Exception& Exception::withFollowUp(Type type, const char* message, const char* file, u32 line, const char* function) noexcept {
        frames.emplace_back(type, message, file, line, function);
        return *this;
    }

    const char* Exception::what() const noexcept {
        static std::string traceMessage;
        if (traceMessage.empty()) {
            for (u32 i = frames.size() - 1; i >= 0; i--) {
                const auto& frame = "#" + std::to_string(i) + " - " + typeToString(frames.at(i).type) + ": " + frames.at(i).file + ":" +
                                    std::to_string(frames.at(i).line) + " | " + frames.at(i).function + ": \"" + frames.at(i).message + "\"\n";
            }
        }
        return traceMessage.c_str();
    }
}  // namespace iodine::core