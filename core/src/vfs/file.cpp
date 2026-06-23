#include "rm/vfs/file.hpp"

namespace rome::core {
    namespace VFS {
        b8 File::accept(Visitor visitor) { return visitor.visit(this); }

    }  // namespace VFS
}  // namespace rome::core
