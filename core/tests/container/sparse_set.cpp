#include "rm/container/sparse_set.hpp"

#include <gtest/gtest.h>

#include <limits>

#include "rm/debug/exception.hpp"
#include "rm/prelude.hpp"

using namespace rome::core;

struct TestStruct {
    rome::i32 a;
    rome::i32 b;
    TestStruct(rome::i32 a, rome::i32 b) : a(a), b(b) {}
    bool operator==(const TestStruct& other) const { return a == other.a && b == other.b; }
};

TEST(SparseSet, Constructors_DefaultCopyMove_OK) {
    SparseSet<char> set;
    EXPECT_FALSE(set.contains(0));
    EXPECT_EQ(set.getSize(), 0);
    set.insert(0, 'p');
    EXPECT_TRUE(set.contains(0));
    EXPECT_EQ(set.getSize(), 1);

    SparseSet<char> copy(set);
    EXPECT_TRUE(copy.contains(0));
    EXPECT_EQ(copy.getSize(), 1);
    copy.insert(1, 'p');
    EXPECT_TRUE(copy.contains(1) && copy.contains(0));
    EXPECT_EQ(copy.getSize(), 2);

    SparseSet<char> move(std::move(copy));
    EXPECT_TRUE(move.contains(1) && move.contains(0));
    EXPECT_EQ(move.getSize(), 2);
    move.insert(2, 'p');
    EXPECT_TRUE(move.contains(2) && move.contains(1) && move.contains(0));
    EXPECT_EQ(move.getSize(), 3);

    auto heap = new SparseSet<char>(move);
    delete heap;
}

TEST(SparseSet, Assignment_DefaultCopyMove_OK) {
    SparseSet<char> set;
    EXPECT_FALSE(set.contains(0));
    EXPECT_EQ(set.getSize(), 0);
    set.insert(0, 'p');
    EXPECT_TRUE(set.contains(0));
    EXPECT_EQ(set.getSize(), 1);

    SparseSet<char> copy = set;
    EXPECT_TRUE(copy.contains(0));
    EXPECT_EQ(copy.getSize(), 1);
    copy.insert(1, 'p');
    EXPECT_TRUE(copy.contains(1) && copy.contains(0));
    EXPECT_EQ(copy.getSize(), 2);

    SparseSet<char> move = std::move(copy);
    EXPECT_TRUE(move.contains(1) && move.contains(0));
    EXPECT_EQ(move.getSize(), 2);
    move.insert(2, 'p');
    EXPECT_TRUE(move.contains(2) && move.contains(1) && move.contains(0));
    EXPECT_EQ(move.getSize(), 3);

    auto heap = new SparseSet<char>(move);
    delete heap;
}

TEST(SparseSet, ContainsSize_Insert_OK) {
    SparseSet<rome::i32> set;
    set.insert(0, 42);
    set.insert(5, 127);

    EXPECT_TRUE(set.contains(0) && set.contains(5));
    EXPECT_FALSE(set.contains(1) || set.contains(4) || set.contains(6));
    EXPECT_EQ(set.getSize(), 2);
}

TEST(SparseSet, DerefIndex_InBounds_OK) {
    SparseSet<rome::i32> set;
    set.insert(0, 42);
    set.insert(5, 127);

    EXPECT_NE(set[0], nullptr);
    EXPECT_NE(set[5], nullptr);
    EXPECT_EQ(*set[0], 42);
    EXPECT_EQ(*set[5], 127);

    // operator[] returns nullptr on miss (and should not throw)
    EXPECT_NO_THROW(set[3]);
    EXPECT_EQ(set[3], nullptr);
}

TEST(SparseSet, At_OutOfBounds_Throws) {
    SparseSet<rome::i32> set;
    set.insert(0, 42);
    set.insert(5, 127);

    EXPECT_EQ(set.at(0), 42);
    EXPECT_EQ(set.at(5), 127);
    EXPECT_THROW(set.at(3), rome::core::Exception);
}

TEST(SparseSet, DerefIterator_InBounds_OK) {
    SparseSet<rome::i32> set;
    set.insert(0, 42);
    set.insert(5, 127);

    EXPECT_EQ(set.begin(), set.find(0));
    EXPECT_EQ(set.begin() + 1, set.find(5));
    EXPECT_EQ(*(set.begin() + 1), 127);
    EXPECT_EQ(*(set.find(5)), 127);
}

TEST(SparseSet, TryInsert_OutOfBounds_Fails) {
    SparseSet<rome::u64> set;
    EXPECT_TRUE(set.try_insert(0, 23));
    EXPECT_FALSE(set.try_insert(static_cast<rome::u64>(std::numeric_limits<rome::u64>::max()), 32));
    EXPECT_EQ(set.getSize(), 1);
}

TEST(SparseSet, TryInsert_InBounds_NoOverwrite) {
    SparseSet<rome::i32> set;

    EXPECT_TRUE(set.try_insert(7, 1));
    EXPECT_FALSE(set.try_insert(7, 2));
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_EQ(set.at(7), 1);
}

TEST(SparseSet, Insert_InBounds_OK) {
    SparseSet<rome::u64> full;
    for (rome::u64 i = 0; i < 500; i++) {
        full.insert(i, i);
    }
    EXPECT_EQ(full.getSize(), 500);

    for (rome::u64 i = 499; i > 0; i--) {
        EXPECT_NO_THROW(full.at(i));
        EXPECT_EQ(full.at(i), i);
    }

    EXPECT_EQ(full.at(0), 0);
}

TEST(SparseSet, Insert_InBounds_Overwrites) {
    SparseSet<rome::i32> set;

    set.insert(7, 127);
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_EQ(set.at(7), 127);
    set.insert(7, 3);
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_EQ(set.at(7), 3);
}

TEST(SparseSet, Find_OutOfBounds_ReturnsEnd) {
    SparseSet<rome::i32> set;
    set.insert(1, 10);

    EXPECT_EQ(set.find(0), set.end());
    EXPECT_NE(set.find(1), set.end());
}

TEST(SparseSet, Emplace_InBounds_NoOverwrite) {
    SparseSet<TestStruct> set;

    set.emplace(3, 1, 2);
    EXPECT_TRUE(set.contains(3));
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_EQ(set.at(3), TestStruct(1, 2));

    set.emplace(3, 9, 9);
    EXPECT_EQ(set.getSize(), 1);
    EXPECT_EQ(set.at(3), TestStruct(1, 2));
}

TEST(SparseSet, Erase_OutOfBounds_OK) {
    SparseSet<rome::i32> set;
    set.insert(10, 100);
    set.insert(20, 200);
    set.insert(30, 300);

    EXPECT_EQ(set.getSize(), 3);

    set.erase(20);
    EXPECT_EQ(set.getSize(), 2);
    EXPECT_FALSE(set.contains(20));
    EXPECT_EQ(set.find(20), set.end());

    // erasing again should be safe
    set.erase(20);
    EXPECT_EQ(set.getSize(), 2);

    // remaining still accessible
    EXPECT_TRUE(set.contains(10));
    EXPECT_TRUE(set.contains(30));
    EXPECT_EQ(set.at(10), 100);
    EXPECT_EQ(set.at(30), 300);
}

TEST(SparseSet, Erase_Always_OK) {
    SparseSet<rome::i32> set;
    for (rome::i32 i = 0; i < 50; i++) {
        set.insert(i, i * 10);
    }
    EXPECT_EQ(set.getSize(), 50);

    set.erase(17);
    EXPECT_FALSE(set.contains(17));
    EXPECT_EQ(set.getSize(), 49);

    for (rome::i32 i = 0; i < 50; ++i) {
        if (i == 17) continue;
        EXPECT_TRUE(set.contains(i)) << "missing index " << i;
        EXPECT_EQ(set.at(i), i * 10) << "wrong value at " << i;
    }
}

TEST(SparseSet, Swap_InBounds_OK) {
    SparseSet<rome::i32> set;
    set.insert(1, 10);
    set.insert(2, 20);
    set.insert(3, 30);

    set.swap(1, 3);

    EXPECT_EQ(set.at(1), 10);
    EXPECT_EQ(set.at(2), 20);
    EXPECT_EQ(set.at(3), 30);
}

TEST(SparseSet, Swap_InBounds_InvalidatesIterators) {
    SparseSet<rome::i32> set;
    set.insert(1, 10);
    set.insert(2, 20);
    set.insert(3, 30);

    auto it1 = set.find(1);
    auto it3 = set.find(3);
    EXPECT_TRUE(it3 > it1);

    set.swap(1, 3);

    it1 = set.find(1);
    it3 = set.find(3);
    EXPECT_TRUE(it3 < it1);
}

TEST(SparseSet, Swap_OutOfBounds_OK) {
    SparseSet<rome::i32> set;
    set.insert(1, 10);
    set.insert(2, 20);
    set.insert(3, 30);

    // following should be no-op
    set.swap(2, 2);       // same index
    set.swap(2, 999);     // one missing
    set.swap(999, 1000);  // both missing

    EXPECT_EQ(set.at(1), 10);
    EXPECT_EQ(set.at(2), 20);
    EXPECT_EQ(set.at(3), 30);
}

TEST(SparseSet, GetData_Always_OK) {
    SparseSet<rome::i32> set;
    auto [ptr0, sz0] = set.getData();
    EXPECT_EQ(sz0, 0);

    set.insert(5, 55);
    set.insert(9, 99);

    auto [ptr, sz] = set.getData();
    EXPECT_EQ(sz, 2);
    EXPECT_NE(ptr, nullptr);

    EXPECT_TRUE((ptr[0] == 55 && ptr[1] == 99) || (ptr[0] == 99 && ptr[1] == 55));
}
