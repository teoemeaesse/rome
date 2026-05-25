#pragma once

#include "rm/prelude.hpp"

namespace rome::core {
    class ECS;

    namespace Plugin {
        using ID = u32;
        inline constexpr ID INVALID_ID = 0;

        inline constexpr const char* LOAD_SYMBOL = "rome_load_plugin";
        inline constexpr const char* UNLOAD_SYMBOL = "rome_unload_plugin";

        using LoadFn = void (*)(ECS&);
        using UnloadFn = void (*)(ECS&);
    }  // namespace Plugin
}  // namespace rome::core
