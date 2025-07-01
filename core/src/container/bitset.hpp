#pragma once

#include "prelude.hpp"

namespace iodine::core {

    /**
     * @brief A flexible bitset that can grow beyond a fixed size.
     * This bitset stores the first 512 bits (8 words) in the stack and spills over to a vector for additional bits.
     * @tparam Word The type of the underlying storage, must be an unsigned integer type (default is u64).
     * @tparam Size The total size of the inner std::bitset, must be a multiple of 64 bits (default is 512 bits).
     *              Spillover to a vector occurs when more than Size bits are set.
     */
    template <typename Word = u64, u64 Size = 512>
    class BitSet {
        STATIC_ASSERT(std::is_unsigned_v<Word>, "Word must be an unsigned integer type");
        static constexpr u64 WordSize = (sizeof(Word) * 8);  ///< Number of bits in a Word (64 for u64).
        STATIC_ASSERT(Size % WordSize == 0, "Size must be a multiple of Word size");

        public:
        BitSet() = default;
        explicit BitSet(Word bits) { resize(bits); }

        /**
         * @brief Performs an in‑place OR with another bitset of identical capacity.
         * @param other The bitset to OR with.
         * @return This.
         */
        BitSet& operator|=(const BitSet& other) {
            IO_ASSERT_MSG(totalWords() == other.totalWords(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < totalWords(); i++) {
                at(i) |= other.at(i);
            }
            return *this;
        }

        /**
         * @brief Performs an in‑place AND with another bitset of identical capacity.
         * @param other The bitset to AND with.
         * @return This.
         */
        BitSet& operator&=(const BitSet& other) {
            IO_ASSERT_MSG(totalWords() == other.totalWords(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < totalWords(); i++) {
                at(i) &= other.at(i);
            }
            return *this;
        }

        /**
         * @brief Returns the bitwise OR of two bitsets with identical capacity.
         * @param left First bitset.
         * @param right Other bitset.
         * @return A new bitset containing "left OR right".
         */
        friend BitSet operator|(BitSet left, const BitSet& right) { return left |= right; }

        /**
         * @brief Returns the bitwise AND of two bitsets with identical capacity.
         * @param left First bitset.
         * @param right Other bitset.
         * @return A new bitset containing "left AND right".
         */
        friend BitSet operator&(BitSet left, const BitSet& right) { return left &= right; }

        /**
         * @brief Tests whether a specific bit is set.
         * @param bit The bit index to test.
         * @return True if the bit is set, false otherwise.
         */
        b8 test(u64 bit) const noexcept { return (*locate(bit) & (1ull << (bit & (WordSize - 1)))) != 0; }

        /**
         * @brief Sets a bit to true.
         * @param bit The bit index to set.
         */
        void set(u64 bit) { mutate(bit, true); }

        /**
         * @brief Sets a bit to false.
         * @param bit The bit index to clear.
         */
        void reset(u64 bit) { mutate(bit, false); }

        /**
         * @brief Toggles a bit.
         * @param bit The bit index to toggle.
         */
        void flip(u64 bit) { mutate(bit, !test(bit)); }

        /**
         * @brief Sets all bits to false.
         */
        void clear() noexcept {
            direct.fill(0);
            std::ranges::fill(spill, 0);
        }

        /**
         * @brief Resizes the bitset to accommodate a new number of bits.
         * @param bits The new size in bits. Should be greater than the current size.
         */
        void resize(u64 bits) {
            if (bits <= Size) return;
            spill.resize((bits - Size + WordSize - 1) / WordSize, 0);
        }

        /**
         * @brief Checks whether any bit is set.
         * @return True if at least one bit is set, false if no bits are set.
         */
        b8 any() const noexcept { return !none(); }

        /**
         * @brief Checks whether no bits are set.
         * @return True if no bits are set, false if at least one bit is set.
         */
        b8 none() const noexcept {
            for (Word word : direct) {
                if (word) return false;
            }
            for (Word word : spill) {
                if (word) return false;
            }
            return true;
        }

        /**
         * @brief Counts the number of bits set.
         * @return The number of bits set.
         */
        u64 count() const noexcept {
            u64 c = 0;
            for (Word word : direct) {
                c += std::popcount(word);
            }
            for (Word word : spill) {
                c += std::popcount(word);
            }
            return c;
        }

        /**
         * @brief Checks whether this bitset intersects with another bitset.
         * @param other The other bitset to check against.
         * @return True if there is at least one bit set in both bitsets, false otherwise.
         */
        b8 intersects(const BitSet& other) const noexcept {
            if (totalWords() != other.totalWords()) return false;

            for (u64 i = 0; i < totalWords(); i++) {
                if (at(i) & other.at(i)) return true;
            }
            return false;
        }

        private:
        std::array<Word, Size / WordSize> direct{};  ///< Stack storage for the first Size bits.
        std::vector<Word> spill;                     ///< Dynamic storage for bits beyond Size.

        /**
         * @brief Calculates the current storage footprint in 64‑bit words.
         * @return The number of words currently owned (stack + spill).
         */
        u64 totalWords() const noexcept { return Size / WordSize + spill.size(); }

        /**
         * @brief Returns a reference to the storage word at index.
         */
        Word& at(Word index) noexcept { return index < Size / WordSize ? direct[index] : spill[index - Size / WordSize]; }
        /**
         * @brief Returns a reference to the storage word at index.
         */
        const Word& at(Word index) const noexcept { return index < Size / WordSize ? direct[index] : spill[index - Size / WordSize]; }

        /**
         * @brief Locates the underlying 64‑bit word that contains a given bit (mutable).
         * @param bit The global bit index to locate.
         * @return A pair of (container pointer, word index) where the bit resides.
         */
        Word* locate(Word bit) noexcept {
            if (bit < Size) return &direct[bit >> 6];

            const Word word = (bit - Size) >> 6;
            if (spill.size() <= word) {
                spill.resize(word + 1, 0);
            }
            return &spill[word];
        }

        /**
         * @brief Locates the underlying 64‑bit word that contains a given bit (const).
         * @param bit The global bit index to locate.
         * @return A pair of (const container pointer, word index) where the bit resides.
         */
        const Word* locate(Word bit) const noexcept {
            if (bit < Size) return &direct[bit >> 6];

            const Word word = (bit - Size) >> 6;
            return &spill[word];
        }

        /**
         * @brief Sets or clears a single bit without bounds checking.
         * @param bit The bit index to modify.
         * @param value True to set the bit, false to clear it.
         */
        void mutate(Word bit, b8 value) {
            Word* w = locate(bit);
            value ? (*w |= 1ull << (bit & (WordSize - 1))) : (*w &= ~(1ull << (bit & (WordSize - 1))));
        }
    };
}  // namespace iodine::core