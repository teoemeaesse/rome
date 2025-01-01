#include "debug/log.h"
#include "platform/platform.h"

#ifdef IO_MACOS

namespace iodine::core {
    const char* Platform::getName() { return PLATFORM_NAME; }

    void Platform::log() { IO_INFOV("Running on {0}", getName()); }

    void Platform::assertCompatible() {
        if (!isMacOS()) {
            IO_FATALV("Incompatible platform: {0}", getName());
            std::exit(EXIT_FAILURE);
        }
    }

    b8 Platform::isWindows() { return false; }

    b8 Platform::isMacOS() { return true; }

    b8 Platform::isUnixLike() { return true; }
}  // namespace iodine::core

#endif