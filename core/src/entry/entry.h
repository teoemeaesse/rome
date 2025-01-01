#pragma once

#include "entry/application.h"

namespace iodine::core {
    static bool shutdownInterrupt = false;

    extern Application* createApplication();

    void handleCtrlC(i32 s);
}  // namespace iodine::core