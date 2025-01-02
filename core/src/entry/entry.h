#pragma once

#include "entry/application.h"

namespace iodine::core {
    extern Application* createApplication();

    void handleCtrlC(i32 s);
}  // namespace iodine::core