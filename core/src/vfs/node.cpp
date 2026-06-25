#include "rm/vfs/node.hpp"

namespace rome::core {
    namespace VFS {
        Node::Node() : segment(""), path("/"), children() {}

        Node::Node(const std::string_view segment) : segment(segment), path(), children() {}

        const std::string_view Node::getSegment() const { return segment; }

        const std::string_view Node::getPath() const { return path; }

        const std::filesystem::path& Node::getMountPath() const { return mountPath; }

        b8 Node::isMounted() const { return !mountPath.empty(); }

        void Node::mount(const std::filesystem::path& path) { mountPath = path; }

        void Node::unmount() { mountPath.clear(); }

        b8 Node::hasChild(const std::string_view segment) const { return children.contains(segment); }

        Node* Node::getChild(const std::string_view segment) {
            auto child = children.find(segment);
            return child == children.end() ? nullptr : child->second.get();
        }

        const Node* Node::getChild(const std::string_view segment) const {
            auto child = children.find(segment);
            return child == children.end() ? nullptr : child->second.get();
        }
    }  // namespace VFS
}  // namespace rome::core
