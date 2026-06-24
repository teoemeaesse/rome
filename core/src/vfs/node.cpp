#include "rm/vfs/node.hpp"

namespace rome::core {
    namespace VFS {
        Node::Node() : segment(""), path("/"), children() {}

        Node::Node(const std::string_view segment) : segment(segment), path(), children() {}

        std::string_view Node::getSegment() const { return segment; }

        std::string_view Node::getPath() const { return path; }

        b8 Node::hasChild(std::string_view segment) const { return children.contains(segment); }

        Node* Node::getChild(std::string_view segment) {
            auto child = children.find(segment);
            return child == children.end() ? nullptr : child->second.get();
        }

        const Node* Node::getChild(std::string_view segment) const {
            auto child = children.find(segment);
            return child == children.end() ? nullptr : child->second.get();
        }
    }  // namespace VFS
}  // namespace rome::core
