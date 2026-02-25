#include "container/sparse_set.hpp"

#include <gtest/gtest.h>

#include <limits>

#include "debug/exception.hpp"
#include "prelude.hpp"

using namespace rome::core;

struct TestStruct {
    int a;
    int b;
    TestStruct(int a, int b) : a(a), b(b) {}
    bool operator==(const TestStruct& other) const { return a == other.a && b == other.b; }
};

TEST(SparseSet, Constructors) {
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

TEST(SparseSet, Operators) {
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

TEST(SparseSet, Functionality) {
    SparseSet<rome::i32> set;
    set.insert(0, 42);
    set.insert(5, 127);
    EXPECT_TRUE(set.contains(0) && set.contains(5));
    EXPECT_FALSE(set.contains(1) || set.contains(4) || set.contains(6));
    EXPECT_EQ(set.getSize(), 2);

    EXPECT_EQ(*set[0], 42);
    EXPECT_EQ(*set[5], 127);
    EXPECT_NO_THROW(set[3]);

    EXPECT_EQ(set.at(0), 42);
    EXPECT_EQ(set.at(5), 127);
    EXPECT_THROW(set.at(3), rome::core::Exception);

    EXPECT_EQ(set.begin(), set.find(0));
    EXPECT_EQ(set.begin() + 1, set.find(5));
    EXPECT_EQ(*(set.begin() + 1), 127);
    EXPECT_EQ(*(set.find(5)), 127);

    SparseSet<rome::u64> full;
    EXPECT_TRUE(full.try_insert(0, 23));
    EXPECT_FALSE(full.try_insert(static_cast<rome::u64>(std::numeric_limits<rome::u64>::max()), 32));
    EXPECT_EQ(set.getSize(), 2);
    for (rome::u64 i = 0; i < 500; i++) {
        full.insert(i, i);
    }
    EXPECT_EQ(full.getSize(), 500);

    for (rome::u64 i = 499; i > 0; i--) {
        EXPECT_NO_THROW(full.at(i));
        EXPECT_EQ(full.at(i), i);
    }
}
