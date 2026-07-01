#pragma once

#include "rm/vfs/node.hpp"

namespace rome::core {
    namespace VFS {
        class File : public Node {
            public:
            b8 insert(Node* node) override;

            b8 accept(Visitor& visitor) override;

            private:
        };
    }  // namespace VFS
}  // namespace rome::core
