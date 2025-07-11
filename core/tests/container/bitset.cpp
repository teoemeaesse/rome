// tests/container/bitset.cpp
#include "container/bitset.hpp"

#include <gtest/gtest.h>

using namespace rome::core;

constexpr rome::u64 InlineBits = 100;
constexpr rome::u64 BigId = InlineBits + 123;  // forces spill resize

#define BITSET_TESTS(ALIAS)                                                                                                \
    TEST(BitSetMinimumFunctionalityTest, SetResetAndTestInline_##ALIAS) {                                                  \
        BitSet<rome::ALIAS, 128> mask;                                                                                     \
        mask.set(3);                                                                                                       \
        mask.set(7);                                                                                                       \
        EXPECT_TRUE(mask.test(3));                                                                                         \
        EXPECT_TRUE(mask.test(7));                                                                                         \
        EXPECT_FALSE(mask.test(10));                                                                                       \
        mask.reset(3);                                                                                                     \
        EXPECT_FALSE(mask.test(3));                                                                                        \
        EXPECT_EQ(mask.count(), 1u);                                                                                       \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, AnyNoneCount_##ALIAS) {                                                                  \
        BitSet<rome::ALIAS, 128> mask;                                                                                     \
        EXPECT_TRUE(mask.none());                                                                                          \
        EXPECT_FALSE(mask.any());                                                                                          \
        EXPECT_EQ(mask.count(), 0u);                                                                                       \
        mask.set(InlineBits - 1);                                                                                          \
        mask.set(BigId);                                                                                                   \
        EXPECT_TRUE(mask.any());                                                                                           \
        EXPECT_FALSE(mask.none());                                                                                         \
        EXPECT_EQ(mask.count(), 2u);                                                                                       \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, SpillResizeAndAccess_##ALIAS) {                                                          \
        BitSet<rome::ALIAS, 128> mask;                                                                                     \
        mask.resize(BigId + 1);                                                                                            \
        mask.set(BigId);                                                                                                   \
        EXPECT_TRUE(mask.test(BigId));                                                                                     \
        EXPECT_EQ(mask.count(), 1u);                                                                                       \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, BinaryOrAndIntersects_##ALIAS) {                                                         \
        BitSet<rome::ALIAS, 128> a, b;                                                                                     \
        a.set(1);                                                                                                          \
        a.set(BigId);                                                                                                      \
        b.set(BigId);                                                                                                      \
        EXPECT_TRUE(a.intersects(b));                                                                                      \
        BitSet<rome::ALIAS, 128> c = a & b;                                                                                \
        EXPECT_TRUE(c.test(BigId));                                                                                        \
        EXPECT_FALSE(c.test(1));                                                                                           \
        EXPECT_EQ(c.count(), 1u);                                                                                          \
        BitSet<rome::ALIAS, 128> d = a | b;                                                                                \
        EXPECT_TRUE(d.test(1));                                                                                            \
        EXPECT_TRUE(d.test(BigId));                                                                                        \
        EXPECT_EQ(d.count(), 2u);                                                                                          \
        d &= c;                                                                                                            \
        EXPECT_EQ(d.count(), 1u);                                                                                          \
        EXPECT_TRUE(d.test(BigId));                                                                                        \
        d |= a;                                                                                                            \
        EXPECT_EQ(d.count(), 2u);                                                                                          \
        EXPECT_TRUE(d.test(1));                                                                                            \
        EXPECT_TRUE(d.test(BigId));                                                                                        \
    }                                                                                                                      \
    TEST(BitSetDeathTest, MismatchedCapacityAssertion_##ALIAS) {                                                           \
        BitSet<rome::ALIAS, 128> small;                                                                                    \
        BitSet<rome::ALIAS, 128> big;                                                                                      \
        big.resize(2048);                                                                                                  \
        EXPECT_DEATH({ small |= big; }, "");                                                                               \
        EXPECT_FALSE(small.intersects(big));                                                                               \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, CopyAndMove_##ALIAS) {                                                                   \
        BitSet<rome::ALIAS, 128> a;                                                                                        \
        a.set(3);                                                                                                          \
        a.set(BigId);                                                                                                      \
        BitSet<rome::ALIAS, 128> copy(a);                                                                                  \
        EXPECT_EQ(copy.count(), 2u);                                                                                       \
        BitSet<rome::ALIAS, 128> moved(std::move(a));                                                                      \
        EXPECT_EQ(moved.count(), 2u);                                                                                      \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, ClearResetsAllBits_##ALIAS) {                                                            \
        BitSet<rome::ALIAS, 128> mask;                                                                                     \
        mask.set(7);                                                                                                       \
        mask.set(BigId);                                                                                                   \
        EXPECT_TRUE(mask.any());                                                                                           \
        EXPECT_EQ(mask.count(), 2u);                                                                                       \
        mask.clear();                                                                                                      \
        EXPECT_TRUE(mask.none());                                                                                          \
        EXPECT_EQ(mask.count(), 0u);                                                                                       \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, FlipTogglesBits_##ALIAS) {                                                               \
        BitSet<rome::ALIAS, 128> mask;                                                                                     \
        mask.flip(42);                                                                                                     \
        EXPECT_TRUE(mask.test(42));                                                                                        \
        mask.flip(42);                                                                                                     \
        EXPECT_FALSE(mask.test(42));                                                                                       \
        mask.flip(BigId);                                                                                                  \
        EXPECT_TRUE(mask.test(BigId));                                                                                     \
        mask.flip(BigId);                                                                                                  \
        EXPECT_FALSE(mask.test(BigId));                                                                                    \
    }                                                                                                                      \
    TEST(BitSetFunctionalityTest, Except_##ALIAS) {                                                                        \
        BitSet<rome::ALIAS, 128> a, b;                                                                                     \
        a.set(1);                                                                                                          \
        a.set(2);                                                                                                          \
        a.set(BigId);                                                                                                      \
        b.set(2);                                                                                                          \
        b.set(BigId);                                                                                                      \
        BitSet<rome::ALIAS, 128> diff = a - b;                                                                             \
        EXPECT_TRUE(diff.test(1));                                                                                         \
        EXPECT_FALSE(diff.test(2));                                                                                        \
        EXPECT_FALSE(diff.test(BigId));                                                                                    \
        EXPECT_EQ(diff.count(), 1u);                                                                                       \
        a -= b;                                                                                                            \
        EXPECT_TRUE(a.test(1));                                                                                            \
        EXPECT_FALSE(a.test(2));                                                                                           \
        EXPECT_FALSE(a.test(BigId));                                                                                       \
        EXPECT_EQ(a.count(), 1u);                                                                                          \
    }                                                                                                                      \
    TEST(BitSetFactoryTest, CreateInitialisesCorrectBits_##ALIAS) {                                                        \
        const rome::ALIAS big_val = static_cast<rome::ALIAS>(BigId);                                                       \
        auto mask = BitSet<rome::ALIAS, 128>::create({static_cast<rome::ALIAS>(1), static_cast<rome::ALIAS>(3), big_val}); \
        EXPECT_TRUE(mask.test(1));                                                                                         \
        EXPECT_TRUE(mask.test(3));                                                                                         \
        EXPECT_TRUE(mask.test(BigId));                                                                                     \
        EXPECT_EQ(mask.count(), 3u);                                                                                       \
    }

BITSET_TESTS(u8)
BITSET_TESTS(u16)
BITSET_TESTS(u32)
BITSET_TESTS(u64)