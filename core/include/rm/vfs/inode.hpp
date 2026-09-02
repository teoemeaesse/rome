#pragma once

#include "rm/prelude.hpp"

namespace rome::core {
    struct Inode {
        enum class Permission : u8 {
            None = 0,
            Read = 1 << 0,
            Write = 1 << 1,
            Execute = 1 << 2,
        };

        u64 blockIndex;
        u8 linkCount = 0;
        Permission permissions = Permission::None;
    };
}  // namespace rome::core
