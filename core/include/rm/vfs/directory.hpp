#pragma once

#include "rm/vfs/node.hpp"

namespace rome::core {
    namespace VFS {
        class Directory : public Node {
            public:
            Directory() = default;
            explicit Directory(const std::string_view segment);

            b8 insert(Node* node) override;

            b8 accept(Visitor& visitor) override;

            private:
        };
    }  // namespace VFS
}  // namespace rome::core
