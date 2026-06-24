#include "rm/vfs/file.hpp"

namespace rome::core {
    namespace VFS {
        b8 Node::SyncVisitor::visit(File* file) {
            if (stack.empty()) stack = file->path;

            file->path = stack;
            return false;
        }

        b8 File::accept(Visitor& visitor) { return visitor.visit(this); }

    }  // namespace VFS
}  // namespace rome::core
