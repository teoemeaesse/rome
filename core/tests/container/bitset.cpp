// tests/container/bitset.cpp
#include "container/bitset.hpp"

#include <gtest/gtest.h>

using namespace iodine::core;

constexpr iodine::u64 InlineBits = 512;
constexpr iodine::u64 BigId = InlineBits + 123;  // forces spill resize

/**
 * @brief Tests basic set / reset / test behaviour inside the stack-allocated block.
 */
TEST(BitSetMinimumFunctionalityTest, SetResetAndTestInline) {
    BitSet<iodine::u32> mask;
    mask.set(3);
    mask.set(7);

    EXPECT_TRUE(mask.test(3));
    EXPECT_TRUE(mask.test(7));
    EXPECT_FALSE(mask.test(10));

    mask.reset(3);
    EXPECT_FALSE(mask.test(3));
    EXPECT_EQ(mask.count(), 1u);  // only bit 7
}

/**
 * @brief Tests that any, none and count work across stack and spill storage.
 */
TEST(BitSetFunctionalityTest, AnyNoneCount) {
    BitSet<iodine::u32> mask;
    EXPECT_TRUE(mask.none());
    EXPECT_FALSE(mask.any());
    EXPECT_EQ(mask.count(), 0u);

    mask.set(InlineBits - 1);  // last inline bit
    mask.set(BigId);           // first spill bit

    EXPECT_TRUE(mask.any());
    EXPECT_FALSE(mask.none());
    EXPECT_EQ(mask.count(), 2u);
}

/**
 * @brief Tests automatic spill resize and access to spill bits.
 */
TEST(BitSetFunctionalityTest, SpillResizeAndAccess) {
    BitSet<iodine::u32> mask;
    mask.resize(BigId + 1);  // ensure capacity
    mask.set(BigId);

    EXPECT_TRUE(mask.test(BigId));
    EXPECT_EQ(mask.count(), 1u);
}

TEST(BitSetFunctionalityTest, BinaryOrAndIntersects) {
    BitSet<iodine::u32> a, b;
    a.set(1);
    a.set(BigId);  // spill bit
    b.set(BigId);

    EXPECT_TRUE(a.intersects(b));

    BitSet<iodine::u32> c = a & b;  // only BigId survives
    EXPECT_TRUE(c.test(BigId));
    EXPECT_FALSE(c.test(1));
    EXPECT_EQ(c.count(), 1u);

    BitSet<iodine::u32> d = a | b;  // union
    EXPECT_TRUE(d.test(1));
    EXPECT_TRUE(d.test(BigId));
    EXPECT_EQ(d.count(), 2u);

    // in-place versions
    d &= c;
    EXPECT_EQ(d.count(), 1u);
    EXPECT_TRUE(d.test(BigId));
}

TEST(BitSetDeathTest, MismatchedCapacityAssertion) {
    BitSet<iodine::u32> small;
    BitSet<iodine::u32> big;
    big.resize(2048);

    EXPECT_DEATH({ small |= big; }, "");
    EXPECT_DEATH({ small &= big; }, "");
    EXPECT_FALSE(small.intersects(big));  // safe: function early-returns
}

TEST(BitSetFunctionalityTest, CopyAndMove) {
    BitSet<iodine::u32> a;
    a.set(3);
    a.set(BigId);

    BitSet<iodine::u32> copy(a);
    EXPECT_EQ(copy.count(), 2u);

    BitSet<iodine::u32> moved(std::move(a));
    EXPECT_EQ(moved.count(), 2u);
}