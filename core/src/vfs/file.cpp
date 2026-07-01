#include "rm/vfs/file.hpp"

namespace rome::core {
    namespace VFS {
        File::File(const std::string_view segment) : Node(segment) {}

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

            if (file->parent == nullptr) return false;
            if (!file->parent->accept(*this)) return false;

            if (!file->segment.empty()) path /= file->segment;
            return true;
        }

        b8 File::insert(Node* node) { return false; }

        b8 File::accept(Visitor& visitor) { return visitor.visit(this); }

    }  // namespace VFS
}  // namespace rome::core
