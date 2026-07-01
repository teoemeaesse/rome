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

class FileVisitorProbe final : public Node::Visitor {
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

TEST(FileTest, Insert_AnyChild_ReturnsFalse) {
    File file;
    File child("child.txt");

    EXPECT_FALSE(file.insert(nullptr));
    EXPECT_FALSE(file.insert(&child));
    EXPECT_EQ(child.parent, nullptr);
}

TEST(FileTest, Accept_WithVisitor_VisitsFile) {
    File file;
    FileVisitorProbe visitor;
    visitor.stop = true;

    EXPECT_TRUE(file.accept(visitor));
    EXPECT_EQ(visitor.visitedFile, &file);
    EXPECT_EQ(visitor.visitedDirectory, nullptr);
}

TEST(SyncVisitorTest, VisitFile_WithExistingStack_UpdatesPath) {
    File file("image.png");

    Node::SyncVisitor visitor;
    visitor.stack = "/assets/";

    EXPECT_FALSE(file.accept(visitor));
    EXPECT_EQ(file.getPath(), "/assets/");
}

TEST(OSPathVisitorTest, VisitFile_WithCloserMount_UsesClosestMountedAncestor) {
    Directory root;
    Directory assets("assets");
    File file("image.png");
    root.mount("/game");

    EXPECT_TRUE(root.insert(&assets));
    EXPECT_TRUE(assets.insert(&file));

    Node::OSPathVisitor visitor;

    EXPECT_TRUE(file.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/game/assets/image.png"));

    assets.mount("/mounted-assets");

    EXPECT_TRUE(file.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/mounted-assets/image.png"));
}

TEST(OSPathVisitorTest, VisitFile_AfterNearestUnmount_FallsBackToAncestorMount) {
    Directory root;
    Directory assets("assets");
    File file("image.png");
    root.mount("/game");
    assets.mount("/mounted-assets");

    EXPECT_TRUE(root.insert(&assets));
    EXPECT_TRUE(assets.insert(&file));

    Node::OSPathVisitor visitor;
    EXPECT_TRUE(file.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/mounted-assets/image.png"));

    assets.unmount();

    EXPECT_TRUE(file.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path("/game/assets/image.png"));
}

TEST(OSPathVisitorTest, VisitFile_WithoutMountedAncestor_FailsAndClearsPath) {
    Directory root;
    File file("image.png");

    EXPECT_TRUE(root.insert(&file));

    Node::OSPathVisitor visitor;
    visitor.path = "/stale/path";

    EXPECT_FALSE(file.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path());
}

TEST(OSPathVisitorTest, VisitFile_AfterFailedResolution_ClearsStalePath) {
    File file("image.png");

    Node::OSPathVisitor visitor;
    visitor.path = "/stale/path";

    EXPECT_FALSE(file.accept(visitor));
    EXPECT_EQ(visitor.getPath(), std::filesystem::path());
}
