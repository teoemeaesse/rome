#include "rm/vfs/node.hpp"

namespace rome::core {
    namespace VFS {
        Node::Node(const std::string_view segment) : segment(segment), children() {}
    }  // namespace VFS
}  // namespace rome::core
