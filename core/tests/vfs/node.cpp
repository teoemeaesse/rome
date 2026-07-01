#include <filesystem>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#define private public
#define protected public
#include "rm/vfs/node.hpp"
#undef protected
#undef private

using namespace rome;
using namespace rome::core;
using namespace rome::core::VFS;

class TestNode final : public Node {
    public:
    using Node::Node;

    b8 insert(Node* node) override { return false; }

    b8 accept(Visitor& visitor) override { return false; }
};

TEST(NodeTest, Constructors_DefaultAndSegment_InitializesPaths) {
    TestNode root;
    TestNode node("asset.txt");

    EXPECT_EQ(root.getSegment(), "");
    EXPECT_EQ(root.getPath(), "/");
    EXPECT_EQ(node.getSegment(), "asset.txt");
    EXPECT_EQ(node.getPath(), "");
}

TEST(NodeTest, MountState_MountAndUnmount_TracksMountPath) {
    TestNode node;

    EXPECT_FALSE(node.isMounted());
    EXPECT_EQ(node.getMountPath(), std::filesystem::path());

    node.mount("/tmp/assets");

    EXPECT_TRUE(node.isMounted());
    EXPECT_EQ(node.getMountPath(), std::filesystem::path("/tmp/assets"));

    node.unmount();

    EXPECT_FALSE(node.isMounted());
    EXPECT_EQ(node.getMountPath(), std::filesystem::path());
}

TEST(OSPathVisitorTest, GetPath_AfterPathSet_ReturnsResolvedPath) {
    Node::OSPathVisitor visitor;
    visitor.path = "/tmp/assets/file.txt";

    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/tmp/assets/file.txt"));
}
