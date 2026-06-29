#pragma once

#include <filesystem>
#include <unordered_map>

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
                /**
                 * @brief Visits a directory node.
                 * @param directory The directory to visit.
                 * @return True if traversal should stop, false otherwise.
                 */
                virtual b8 visit(Directory* directory) = 0;

                /**
                 * @brief Visits a file node.
                 * @param file The file to visit.
                 * @return True if traversal should stop, false otherwise.
                 */
                virtual b8 visit(File* file) = 0;
            };

            class SyncVisitor final : public Visitor {
                public:
                /** @inheritdoc */
                b8 visit(Directory* directory) override;

                /** @inheritdoc */
                b8 visit(File* file) override;

                private:
                std::string stack;  ///< The current virtual path.
            };

            class OSPathVisitor final : public Visitor {
                public:
                /** @inheritdoc */
                b8 visit(Directory* directory) override;

                /** @inheritdoc */
                b8 visit(File* file) override;

                /**
                 * @brief Gets the resolved OS path.
                 * @return The resolved OS path.
                 */
                const std::filesystem::path& getPath() const;

                private:
                std::filesystem::path path;  ///< The resolved OS path.
            };

            Node();
            Node(const std::string_view segment);
            Node(const Node&) = default;
            Node(Node&&) = default;
            Node& operator=(const Node&) = default;
            Node& operator=(Node&&) = default;

            /**
             * @brief Gets the segment for this node.
             * @return The segment for this node.
             */
            const std::string_view getSegment() const;

            /**
             * @brief Gets the path for this node.
             * @return The path for this node.
             */
            const std::string_view getPath() const;

            /**
             * @brief Gets the mount path for this node.
             * @return The mount path for this node.
             */
            const std::filesystem::path& getMountPath() const;

            /**
             * @brief Checks if this node is mounted.
             * @return True if this node is mounted, false otherwise.
             */
            b8 isMounted() const;

            /**
             * @brief Mounts this node to an OS path.
             * @param path The OS path to mount.
             */
            void mount(const std::filesystem::path& path);

            /**
             * @brief Unmounts this node.
             */
            void unmount();

            /**
             * @brief Checks if this node has a child with the given segment.
             * @param segment The segment to check.
             * @return True if this node has the child, false otherwise.
             */
            b8 hasChild(const std::string_view segment) const;

            /**
             * @brief Gets a child by segment.
             * @param segment The segment of the child.
             * @return A pointer to the child, or nullptr if not found.
             */
            Node* getChild(const std::string_view segment);

            /**
             * @brief Gets a child by segment.
             * @param segment The segment of the child.
             * @return A const pointer to the child, or nullptr if not found.
             */
            const Node* getChild(const std::string_view segment) const;

            /**
             * @brief Visits this node.
             * @param visitor The node visitor.
             * @return True if traversal should stop, false otherwise.
             */
            virtual b8 accept(Visitor& visitor) = 0;

            protected:
            std::unordered_map<std::string, Shared<Node>, TransparentSVHash, std::equal_to<>> children;  ///< This node's direct children.

            private:
            friend class SyncVisitor;
            friend class OSPathVisitor;

            std::string segment;              ///< The most basic unit of composition for a VFS path.
            std::string path;                 ///< The virtual path up to this node.
            std::filesystem::path mountPath;  ///< The path this node is mounted to.
            Weak<Node> parent;                ///< This node's parent.
        };
    }  // namespace VFS
}  // namespace rome::core
