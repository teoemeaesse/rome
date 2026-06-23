#include "rm/vfs/directory.hpp"

namespace rome::core {
    namespace VFS {
        b8 Directory::accept(Visitor visitor) {
            for (auto& [_, child] : children) {
                child->accept(visitor);
            }
            return visitor.visit(this);
        }
    }  // namespace VFS
}  // namespace rome::core
