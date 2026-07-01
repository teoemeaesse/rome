#include <filesystem>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#define private public
#define protected public
#include "rm/vfs/directory.hpp"
#include "rm/vfs/file.hpp"
#undef protected
#undef private

using namespace rome;
using namespace rome::core;
using namespace rome::core::VFS;

class DirectoryVisitorProbe final : public Node::Visitor {
    public:
    b8 visit(Directory* directory) override {
        visitedDirectory = directory;
        return stop;
    }

    b8 visit(File* file) override {
        visitedFile = file;
        return stop;
    }

    Directory* visitedDirectory = nullptr;
    File* visitedFile = nullptr;
    b8 stop = false;
};

TEST(DirectoryTest, Insert_ValidChild_StoresChildAndSetsParent) {
    Directory directory;
    File file("asset.txt");

    EXPECT_TRUE(directory.insert(&file));

    EXPECT_EQ(file.parent, &directory);
    EXPECT_EQ(directory.children.size(), 1u);
    EXPECT_EQ(directory.children["asset.txt"], &file);
}

TEST(DirectoryTest, Insert_InvalidChild_ReturnsFalse) {
    Directory directory;
    File file("asset.txt");
    File duplicate("asset.txt");

    EXPECT_FALSE(directory.insert(nullptr));

    File emptySegment;
    EXPECT_FALSE(directory.insert(&emptySegment));

    EXPECT_TRUE(directory.insert(&file));
    EXPECT_FALSE(directory.insert(&duplicate));
    EXPECT_EQ(duplicate.parent, nullptr);
}

TEST(DirectoryTest, Insert_SameNode_ReturnsFalse) {
    Directory directory;
    File file("asset.txt");

    EXPECT_TRUE(directory.insert(&file));
    EXPECT_FALSE(directory.insert(&file));
    EXPECT_EQ(file.parent, &directory);
    EXPECT_EQ(directory.children.size(), 1u);
}

TEST(DirectoryTest, Insert_DuplicateSegment_DoesNotMutateTree) {
    Directory directory;
    File file("asset.txt");
    File duplicate("asset.txt");

    EXPECT_TRUE(directory.insert(&file));
    EXPECT_FALSE(directory.insert(&duplicate));

    EXPECT_EQ(file.parent, &directory);
    EXPECT_EQ(duplicate.parent, nullptr);
    EXPECT_EQ(directory.children.size(), 1u);
    EXPECT_EQ(directory.children["asset.txt"], &file);
}

TEST(DirectoryTest, Insert_AlreadyParentedChild_ReturnsFalse) {
    Directory first;
    Directory second;
    File file("asset.txt");

    EXPECT_TRUE(first.insert(&file));
    EXPECT_FALSE(second.insert(&file));

    EXPECT_EQ(file.parent, &first);
    EXPECT_EQ(first.children.size(), 1u);
    EXPECT_EQ(first.children["asset.txt"], &file);
    EXPECT_TRUE(second.children.empty());
}

TEST(DirectoryTest, Accept_WithVisitor_VisitsDirectory) {
    Directory directory;
    DirectoryVisitorProbe visitor;
    visitor.stop = true;

    EXPECT_TRUE(directory.accept(visitor));
    EXPECT_EQ(visitor.visitedDirectory, &directory);
    EXPECT_EQ(visitor.visitedFile, nullptr);
}

TEST(SyncVisitorTest, VisitDirectory_WithNestedChildren_UpdatesChildPaths) {
    Directory root;
    Directory directory("assets");
    File file("image.png");

    EXPECT_TRUE(root.insert(&directory));
    EXPECT_TRUE(directory.insert(&file));

    Node::SyncVisitor visitor;
    EXPECT_FALSE(root.accept(visitor));

    EXPECT_EQ(root.getPath(), "/");
    EXPECT_EQ(directory.getPath(), "/");
    EXPECT_EQ(file.getPath(), "/assets/");
}

TEST(OSPathVisitorTest, VisitDirectory_WithCloserMount_UsesClosestMountedAncestor) {
    Directory root;
    Directory assets("assets");
    root.mount("/game");

    EXPECT_TRUE(root.insert(&assets));

    Node::OSPathVisitor visitor;

    EXPECT_TRUE(assets.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/game/assets"));

    assets.mount("/mounted-assets");

    EXPECT_TRUE(assets.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/mounted-assets"));
}

TEST(OSPathVisitorTest, VisitDirectory_AfterRejectedReparent_KeepsOriginalResolution) {
    Directory first;
    Directory second;
    Directory assets("assets");
    first.mount("/first");
    second.mount("/second");

    EXPECT_TRUE(first.insert(&assets));
    EXPECT_FALSE(second.insert(&assets));

    Node::OSPathVisitor visitor;
    EXPECT_TRUE(assets.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/first/assets"));
}
