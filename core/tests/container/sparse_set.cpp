#include "container/sparse_set.hpp"

#include <gtest/gtest.h>

using namespace rome::core;

struct TestStruct {
    int a;
    int b;
    TestStruct(int a, int b) : a(a), b(b) {}
    bool operator==(const TestStruct& other) const { return a == other.a && b == other.b; }
};

/**
 * @brief Tests basic creation, insertion, and element access.
 */
TEST(SparseSetMinimumFunctionalityTest, CreateDestroy) {
    SparseSet<char> set;
    set.insert(0, 'x');
    set.insert(1, 'y');

    EXPECT_EQ(set.at(0), 'x');
    EXPECT_EQ(set[0], 'x');
    EXPECT_EQ(set.at(1), 'y');
    EXPECT_EQ(set[1], 'y');

    EXPECT_EQ(set.getSize(), 2);
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.contains(2));
}

/**
 * @brief Tests inserting multiple values and checking their existence.
 */
TEST(SparseSetFunctionalityTest, InsertAndContains) {
    SparseSet<int> set;
    set.insert(0, 10);
    set.insert(5, 50);
    set.insert(10, 100);

    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(5));
    EXPECT_TRUE(set.contains(10));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(2));
    EXPECT_EQ(set.getSize(), 3);

    EXPECT_EQ(set.at(0), 10);
    EXPECT_EQ(set.at(5), 50);
    EXPECT_EQ(set.at(10), 100);
}

/**
 * @brief Tests that duplicate insertions (using the same index) do not overwrite the original value.
 */
TEST(SparseSetFunctionalityTest, InsertDuplicate) {
    SparseSet<char> set;
    set.insert(2, 'a');
    EXPECT_EQ(set.getSize(), 1);

    // Inserting at the same index should do nothing.
    set.insert(2, 'b');
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_EQ(set.at(2), 'a');
}

/**
 * @brief Tests erasing an element from the sparse set.
 */
TEST(SparseSetFunctionalityTest, EraseElement) {
    SparseSet<char> set;
    set.insert(0, 'a');
    set.insert(1, 'b');
    set.insert(2, 'c');

    EXPECT_EQ(set.getSize(), 3);
    EXPECT_TRUE(set.contains(1));

    set.erase(1);
    EXPECT_EQ(set.getSize(), 2);
    EXPECT_FALSE(set.contains(1));

    // Ensure that the remaining elements are still accessible.
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(2));
    EXPECT_EQ(set.at(0), 'a');
    EXPECT_EQ(set.at(2), 'c');
}

/**
 * @brief Tests erasing a non-existent element (should have no effect).
 */
TEST(SparseSetFunctionalityTest, EraseNonExistent) {
    SparseSet<int> set;
    set.insert(3, 30);
    set.erase(5);
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_TRUE(set.contains(3));
    EXPECT_FALSE(set.contains(5));
}

/**
 * @brief Tests constructing elements in place using emplace.
 */
TEST(SparseSetFunctionalityTest, EmplaceElement) {
    SparseSet<TestStruct> set;
    set.emplace(7, 3, 4);
    EXPECT_TRUE(set.contains(7));

    const TestStruct& ts = set.at(7);
    EXPECT_EQ(ts.a, 3);
    EXPECT_EQ(ts.b, 4);
}

/**
 * @brief Tests that the non-const operator[] can be used both to read and modify elements.
 */
TEST(SparseSetFunctionalityTest, OperatorAccessAndModification) {
    SparseSet<std::string> set;
    set.insert(10, "Hello");
    EXPECT_EQ(set[10], "Hello");

    // Modify the element.
    set[10] = "World";
    EXPECT_EQ(set[10], "World");
}

/**
 * @brief Tests iterator support, both explicit iterators and range-based for loops.
 */
TEST(SparseSetFunctionalityTest, IteratorFunctionality) {
    SparseSet<int> set;
    set.insert(1, 10);
    set.insert(2, 20);
    set.insert(3, 30);

    int sum = 0;
    for (auto it = set.begin(); it != set.end(); it++) {
        sum += *it;
    }
    EXPECT_EQ(sum, 60);

    sum = 0;
    for (const auto& value : set) {
        sum += value;
    }
    EXPECT_EQ(sum, 60);
}

/**
 * @brief Tests that the copy constructor creates an equivalent sparse set.
 */
TEST(SparseSetFunctionalityTest, CopyConstructor) {
    SparseSet<char> set;
    set.insert(0, 'x');
    set.insert(1, 'y');

    SparseSet<char> copySet(set);
    EXPECT_EQ(copySet.getSize(), set.getSize());
    EXPECT_TRUE(copySet.contains(0));
    EXPECT_TRUE(copySet.contains(1));
    EXPECT_EQ(copySet.at(0), 'x');
    EXPECT_EQ(copySet.at(1), 'y');
}

/**
 * @brief Tests that the move constructor transfers the data correctly.
 */
TEST(SparseSetFunctionalityTest, MoveConstructor) {
    SparseSet<int> set;
    set.insert(0, 100);
    set.insert(2, 200);

    SparseSet<int> movedSet(std::move(set));
    EXPECT_EQ(movedSet.getSize(), 2);
    EXPECT_TRUE(movedSet.contains(0));
    EXPECT_TRUE(movedSet.contains(2));
    EXPECT_EQ(movedSet.at(0), 100);
    EXPECT_EQ(movedSet.at(2), 200);

    // The moved-from set is valid but its state is unspecified.
}

/**
 * @brief Tests copy assignment.
 */
TEST(SparseSetFunctionalityTest, CopyAssignment) {
    SparseSet<char> set;
    set.insert(1, 'a');
    set.insert(3, 'b');

    SparseSet<char> copySet;
    copySet = set;

    EXPECT_EQ(copySet.getSize(), set.getSize());
    EXPECT_TRUE(copySet.contains(1));
    EXPECT_TRUE(copySet.contains(3));
    EXPECT_EQ(copySet.at(1), 'a');
    EXPECT_EQ(copySet.at(3), 'b');
}

/**
 * @brief Tests move assignment.
 */
TEST(SparseSetFunctionalityTest, MoveAssignment) {
    SparseSet<int> set;
    set.insert(5, 500);
    set.insert(6, 600);

    SparseSet<int> movedSet;
    movedSet = std::move(set);

    EXPECT_EQ(movedSet.getSize(), 2);
    EXPECT_TRUE(movedSet.contains(5));
    EXPECT_TRUE(movedSet.contains(6));
    EXPECT_EQ(movedSet.at(5), 500);
    EXPECT_EQ(movedSet.at(6), 600);
}

/**
 * @brief Tests that swap only reorders the raw data buffer (via getData), but leaves the logical reference intact.
 */
TEST(SparseSetFunctionalityTest, SwapOnlyAffectsRawBuffer) {
    SparseSet<int> set;
    set.insert(1, 10);
    set.insert(2, 20);

    // Capture raw data before swap
    auto [ptr_before, size_before] = set.getData();
    ASSERT_EQ(size_before, 2u);
    EXPECT_EQ(ptr_before[0], 10);
    EXPECT_EQ(ptr_before[1], 20);

    // Logical access also before
    EXPECT_EQ(set.at(1), 10);
    EXPECT_EQ(set.at(2), 20);

    // Perform swap
    set.swap(1, 2);

    // Capture raw data after swap
    auto [ptr_after, size_after] = set.getData();
    EXPECT_EQ(size_after, size_before);

    // The pointer itself stays valid (same buffer)
    EXPECT_EQ(ptr_after, ptr_before);

    // But the raw contents have been flipped
    EXPECT_EQ(ptr_after[0], 20);
    EXPECT_EQ(ptr_after[1], 10);

    // Logical access is still consistent with original values
    EXPECT_EQ(set.at(1), 10);
    EXPECT_EQ(set.at(2), 20);
}
