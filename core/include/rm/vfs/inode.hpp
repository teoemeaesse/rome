#pragma once

#include "rm/prelude.hpp"

namespace rome::core {
    struct Inode {
        enum class Type : u8 {
            File = 0,
            Directory,
        };

        enum class Permission : u8 {
            None = 0,
            Read = 1 << 0,
            Write = 1 << 1,
            Execute = 1 << 2,
        };

        std::chrono::system_clock::time_point ctime;  ///< Created time.
        std::chrono::system_clock::time_point mtime;  ///< Last modification time.
        std::chrono::system_clock::time_point atime;  ///< Last access time.
        u64 blockIndex = 0;                           ///< An index into a volatile memory block.
        u8 linkCount = 0;                             ///< Number of hard links to this inode.
        Permission permissions = Permission::None;    ///< Read, write and execute permissions.
        Type type = Type::File;                       ///< Node type.
    };
}  // namespace rome::core
