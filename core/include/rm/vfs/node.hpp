#pragma once

#include <map>

#include "rm/prelude.hpp"

namespace rome::core {
    namespace VFS {
        class Directory;
        class File;

        /**
         * @brief A node recursively defines the structure of the VFS, storing all metadata related to a file, directory, mount etc.
         */
        class Node {
            public:
            class Visitor {
                public:
                virtual b8 visit(Directory* directory);
                virtual b8 visit(File* directory);
            };

            Node(const std::string_view segment);
            Node(const Node&) = default;
            Node(Node&&) = default;
            Node& operator=(const Node&) = default;
            Node& operator=(Node&&) = default;

            /**
             * @brief Visits this node, recursively performing a generic operation on it.
             * @param visitor The node visitor.
             * @return True if the visitor succeeded, false otherwise.
             */
            virtual b8 accept(Visitor visitor) = 0;

            protected:
            std::map<std::string, Unique<Node>> children;  ///< This node's direct children.

            private:
            std::string segment;  ///< The most basic unit of composition for a VFS path.
        };
    }  // namespace VFS
}  // namespace rome::core
