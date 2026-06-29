#include "rm/vfs/file.hpp"

namespace rome::core {
    namespace VFS {
        b8 Node::SyncVisitor::visit(File* file) {
            if (stack.empty()) stack = file->path;

            file->path = stack;
            return false;
        }

        b8 Node::OSPathVisitor::visit(File* file) {
            path.clear();

            if (file->isMounted()) {
                path = file->mountPath;
                return true;
            }

            Shared<Node> parent = file->parent.lock();
            if (parent == nullptr) return false;
            if (!parent->accept(*this)) return false;

            if (!file->segment.empty()) path /= file->segment;
            return true;
        }

        b8 File::accept(Visitor& visitor) { return visitor.visit(this); }

    }  // namespace VFS
}  // namespace rome::core
