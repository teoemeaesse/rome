#pragma once

#include "prelude.hpp"

namespace iodine::core {

    /**
     * @brief A flexible bitset that can grow beyond a fixed size.
     * This bitset stores the first 512 bits (8 words) in the stack and spills over to a vector for additional bits.
     * @tparam Size The total size of the inner std::bitset, must be a multiple of 64 bits (default is 512 bits).
     *              Spillover to a vector occurs when more than Size bits are set.
     */
    template <u64 Size = 512>
    class BitSet {
        STATIC_ASSERT(Size % 64 == 0, "Size must be a multiple of 64");

        public:
        BitSet() = default;
        explicit BitSet(u64 bits) { resize(bits); }

        /**
         * @brief Performs an in‑place OR with another bitset of identical capacity.
         * @param other The bitset to OR with.
         * @return This.
         */
        BitSet& operator|=(const BitSet& other) {
            STATIC_ASSERT(totalWords() == other.totalWords(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < Size / 64; i++) {
                direct[i] |= other.direct[i];
            }

            u64 otherSize = other.spill.size();
            if (spill.size() < otherSize) {
                spill.resize(otherSize, 0);
            }
            for (u64 i = 0; i < otherSize; i++) {
                spill[i] |= other.spill[i];
            }
            return *this;
        }

        /**
         * @brief Performs an in‑place AND with another bitset of identical capacity.
         * @param other The bitset to AND with.
         * @return This.
         */
        BitSet& operator&=(const BitSet& other) {
            STATIC_ASSERT(totalWords() == other.totalWords(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < Size / 64; i++) {
                direct[i] &= other.direct[i];
            }

            const auto shared = std::min(spill.size(), other.spill.size());
            for (u64 i = 0; i < shared; i++) {
                spill[i] &= other.spill[i];
            }
            for (u64 i = shared; i < spill.size(); i++) {
                spill[i] = 0;
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
        b8 test(u64 bit) const noexcept {
            const auto [vec, index] = locate(bit);
            return (*vec)[index] & mas k(bit);
        }

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
         * @brief Returns a mask for the specified bit.
         * @param bit The bit index.
         * @return A mask with the specified bit set.
         */
        void resize(u64 bits) {
            if (bits <= Size) return;
            spill.resize((bits - Size + 63) / 64, 0);
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
            for (u64 word : direct) {
                if (word) return false;
            }
            for (u64 word : spill) {
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
            for (u64 word : direct) {
                c += std::popcount(word);
            }
            for (u64 word : spill) {
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

            for (u64 i = 0; i < Size / 64; i++) {
                if (direct[i] & other.direct[i]) return true;
            }

            const u64 shared = std::min(spill.size(), other.spill.size());
            for (u64 i = 0; i < shared; i++) {
                if (spill[i] & other.spill[i]) return true;
            }
            return false;
        }

        private:
        std::array<u64, Size / 64> direct{};  ///< Stack storage for the first Size bits.
        std::vector<u64> spill;               ///< Dynamic storage for bits beyond Size.

        /**
         * @brief Calculates the current storage footprint in 64‑bit words.
         * @return The number of words currently owned (inline + spill).
         */
        u64 totalWords() const noexcept { return Size / 64 + spill.size(); }

        /**
         * @brief Locates the underlying 64‑bit word that contains a given bit (mutable).
         * @param bit The global bit index to locate.
         * @return A pair of (container pointer, word index) where the bit resides.
         * @note Grows the spill vector if the bit lies beyond current capacity.
         */
        std::pair<std::vector<Word>*, u64> locate(u64 bit) noexcept {
            if (bit < Size) return std::pair{reinterpret_cast<std::vector<Word>*>(&direct), bit >> 6};

            const auto index = (bit - Size) >> 6;
            if (spill.size() < index) {
                spill.resize(index, 0);
            }
            return std::pair{&spill, index};
        }

        /**
         * @brief Locates the underlying 64‑bit word that contains a given bit (const).
         * @param bit The global bit index to locate.
         * @return A pair of (const container pointer, word index) where the bit resides.
         */
        std::pair<std::vector<Word>*, u64> locate(u64 bit) const noexcept {
            if (bit < Size) return std::pair{reinterpret_cast<const std::vector<Word>*>(&direct), bit >> 6};
            return std::pair{&spill, (bit - Size) >> 6};
        }

        /**
         * @brief Sets or clears a single bit without bounds checking.
         * @param bit   The bit index to modify.
         * @param value True to set the bit, false to clear it.
         */
        void mutate(u64 bit, b8 value) {
            auto [vec, word] = locate(bit);
            Word& ref = (*vec)[word];
            if (value) {
                ref |= 1 << (bit & 63);
            } else {
                ref &= ~(1 << (bit & 63));
            }
        }
    };
}  // namespace iodine::core