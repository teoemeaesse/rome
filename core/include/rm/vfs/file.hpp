#pragma once

#include "rm/vfs/node.hpp"

namespace rome::core {
    namespace VFS {
        class File : public Node {
            public:
            File() = default;
            explicit File(const std::string_view segment);

            b8 insert(Node* node) override;

            b8 accept(Visitor& visitor) override;

            private:
        };
    }  // namespace VFS
}  // namespace rome::core
