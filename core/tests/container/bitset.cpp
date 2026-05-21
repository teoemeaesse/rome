#include "rm/container/bitset.hpp"

#include <gtest/gtest.h>

using namespace rome::core;

constexpr rome::u64 InlineBits = 128;
constexpr rome::u64 BigId = InlineBits + 123;  // forces spill resize

TEST(BitSet, Constructors_DefaultCopyMove_OK) {
    BitSet<InlineBits> mask;
    EXPECT_FALSE(mask.test(127));
    EXPECT_EQ(mask.count(), 0u);
    mask.set(127);
    EXPECT_TRUE(mask.test(127));
    EXPECT_EQ(mask.count(), 1u);

    BitSet<InlineBits> list({1, 3, 5, 7, 11, 13});
    EXPECT_TRUE(list.test(1));
    EXPECT_TRUE(list.test(3));
    EXPECT_TRUE(list.test(5));
    EXPECT_TRUE(list.test(7));
    EXPECT_TRUE(list.test(11));
    EXPECT_TRUE(list.test(13));

    BitSet<InlineBits> copy(mask);
    EXPECT_TRUE(copy.test(127));
    EXPECT_EQ(copy.count(), 1u);
    copy.set(42);
    EXPECT_TRUE(copy.test(42));
    EXPECT_EQ(copy.count(), 2u);

    BitSet<InlineBits> move(std::move(copy));
    EXPECT_TRUE(move.test(42));
    EXPECT_EQ(move.count(), 2u);
    move.set(13);
    EXPECT_TRUE(move.test(13));
    EXPECT_EQ(move.count(), 3u);
}

TEST(BitSet, Assignment_DefaultCopyMove_OK) {
    BitSet<InlineBits> mask;
    EXPECT_FALSE(mask.test(127));
    EXPECT_EQ(mask.count(), 0u);
    mask.set(127);
    EXPECT_TRUE(mask.test(127));
    EXPECT_EQ(mask.count(), 1u);

    BitSet<InlineBits> copy = mask;
    EXPECT_TRUE(copy.test(127));
    EXPECT_EQ(copy.count(), 1u);
    copy.set(42);
    EXPECT_TRUE(copy.test(42));
    EXPECT_EQ(copy.count(), 2u);

    BitSet<InlineBits> move = std::move(copy);
    EXPECT_TRUE(move.test(42));
    EXPECT_EQ(move.count(), 2u);
    move.set(13);
    EXPECT_TRUE(move.test(13));
    EXPECT_EQ(move.count(), 3u);
}

TEST(BitSet, SetReset_Always_OK) {
    BitSet<InlineBits> mask;
    mask.set(3);
    mask.set(7);
    EXPECT_TRUE(mask.test(3));
    EXPECT_TRUE(mask.test(7));
    EXPECT_FALSE(mask.test(10));
    mask.reset(3);
    EXPECT_FALSE(mask.test(3));
    EXPECT_EQ(mask.count(), 1u);
}

TEST(BitSet, AnyNoneCount_Always_OK) {
    BitSet<InlineBits> mask;
    EXPECT_TRUE(mask.none());
    EXPECT_FALSE(mask.any());
    EXPECT_EQ(mask.count(), 0u);
    mask.set(InlineBits - 1);
    mask.set(BigId);
    EXPECT_TRUE(mask.any());
    EXPECT_FALSE(mask.none());
    EXPECT_EQ(mask.count(), 2u);
}

TEST(BitSet, SetReset_Spills_Resizes) {
    BitSet<InlineBits> mask;
    mask.set(BigId);
    mask.set(BigId + 100);
    EXPECT_TRUE(mask.test(BigId));
    EXPECT_TRUE(mask.test(BigId + 100));
    EXPECT_EQ(mask.count(), 2u);
}

TEST(BitSet, AndOrIntersects_MatchingWidth_OK) {
    BitSet<InlineBits> a, b;
    a.set(1);
    b.set(BigId);
    EXPECT_FALSE(a.intersects(b));
    a.set(BigId);
    EXPECT_TRUE(a.intersects(b));
    BitSet<InlineBits> c = a & b;
    EXPECT_TRUE(c.test(BigId));
    EXPECT_FALSE(c.test(1));
    EXPECT_EQ(c.count(), 1u);
    BitSet<InlineBits> d = a | b;
    EXPECT_TRUE(d.test(1));
    EXPECT_TRUE(d.test(BigId));
    EXPECT_EQ(d.count(), 2u);
    d &= c;
    EXPECT_EQ(d.count(), 1u);
    EXPECT_TRUE(d.test(BigId));
    d |= a;
    EXPECT_EQ(d.count(), 2u);
    EXPECT_TRUE(d.test(1));
    EXPECT_TRUE(d.test(BigId));
}

TEST(BitSet, AndOrIntersectsExcept_DifferentWidth_Fails) {
    BitSet<InlineBits> small;
    BitSet<InlineBits> big;
    big.resize(2048);
    EXPECT_DEATH(auto x = small | big, "");
    EXPECT_DEATH(small |= big, "");
    EXPECT_DEATH(auto x = small & big, "");
    EXPECT_DEATH(small &= big, "");
    EXPECT_DEATH(auto x = small - big, "");
    EXPECT_DEATH(small -= big, "");
    EXPECT_FALSE(small.intersects(big));
}

TEST(BitSet, Clear_Always_OK) {
    BitSet<InlineBits> mask;
    mask.set(7);
    mask.set(BigId);
    EXPECT_TRUE(mask.any());
    EXPECT_EQ(mask.count(), 2u);
    mask.clear();
    EXPECT_TRUE(mask.none());
    EXPECT_EQ(mask.count(), 0u);
}

TEST(BitSet, Flip_Always_OK) {
    BitSet<InlineBits> mask;
    mask.flip(42);
    EXPECT_TRUE(mask.test(42));
    mask.flip(42);
    EXPECT_FALSE(mask.test(42));
    mask.flip(BigId);
    EXPECT_TRUE(mask.test(BigId));
    mask.flip(BigId);
    EXPECT_FALSE(mask.test(BigId));
}

TEST(BitSet, Except_MatchingWidth_OK) {
    BitSet<InlineBits> a, b;
    a.set(1);
    a.set(2);
    a.set(BigId);
    b.set(2);
    b.set(BigId);
    BitSet<InlineBits> diff = a - b;
    EXPECT_TRUE(diff.test(1));
    EXPECT_FALSE(diff.test(2));
    EXPECT_FALSE(diff.test(BigId));
    EXPECT_EQ(diff.count(), 1u);
    a -= b;
    EXPECT_TRUE(a.test(1));
    EXPECT_FALSE(a.test(2));
    EXPECT_FALSE(a.test(BigId));
    EXPECT_EQ(a.count(), 1u);
}
