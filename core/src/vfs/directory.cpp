#include "rm/vfs/directory.hpp"

namespace rome::core {
    namespace VFS {
        b8 Node::SyncVisitor::visit(Directory* directory) {
            if (stack.empty()) stack = directory->path;

            const std::string parent = stack;
            if (!directory->segment.empty()) stack += directory->segment + "/";
            directory->path = parent;

            for (auto& [_, child] : directory->children) {
                if (child->accept(*this)) {
                    stack = parent;
                    return true;
                }
            }

            stack = parent;
            return false;
        }

        b8 Node::OSPathVisitor::visit(Directory* directory) {
            path.clear();

            if (directory->isMounted()) {
                path = directory->mountPath;
                return true;
            }

            Shared<Node> parent = directory->parent.lock();
            if (parent == nullptr) return false;
            if (!parent->accept(*this)) return false;

            if (!directory->segment.empty()) path /= directory->segment;
            return true;
        }

        b8 Directory::accept(Visitor& visitor) { return visitor.visit(this); }
    }  // namespace VFS
}  // namespace rome::core
