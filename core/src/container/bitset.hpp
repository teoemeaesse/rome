#pragma once

#include "prelude.hpp"

namespace rome::core {

    /**
     * @brief A flexible bitset that can grow beyond a fixed size.
     * This bitset stores the first 512 bits (8 words) in the stack and spills over to a vector for additional bits.
     * @tparam Alias An optional type used to index into the bitset, must be an unsigned integer type no larger than 64 bits (default is u64).
     * @tparam Size The maximum expected capacity, must be a multiple of Alias' width in bits (default is 512 bits).
     *              You can go over this size, falling back to dynamic storage.
     */
    template <typename Alias = u64, u64 Size = 512>
    class RM_API BitSet final {
        STATIC_ASSERT(std::is_unsigned_v<Alias>, "Alias must be an unsigned integer type");
        STATIC_ASSERT(sizeof(Alias) <= 8, "Alias must be no larger than 64 bits");
        STATIC_ASSERT(Size % (sizeof(Alias) * 8) == 0, "Size must be a multiple of Alias' width");

        public:
        BitSet() = default;
        explicit BitSet(u64 size) { resize(size); }
        BitSet(std::initializer_list<Alias> bits) {
            for (Alias bit : bits) {
                set(bit);
            }
        }
        BitSet(const BitSet&) = default;
        BitSet(BitSet&&) = default;
        BitSet& operator=(const BitSet&) = default;
        BitSet& operator=(BitSet&&) = default;
        ~BitSet() = default;

        /**
         * @brief Creates a bitset from a list of bits.
         * @param bits The bits to set.
         * @return A new bitset with the specified bits set.
         */
        static BitSet create(std::initializer_list<Alias> bits) {
            BitSet set(Size);
            for (Alias bit : bits) {
                set.set(bit);
            }
            return set;
        }

        /**
         * @brief Performs an in‑place OR with another bitset of identical capacity.
         * @param other The bitset to OR with.
         * @return This.
         */
        BitSet& operator|=(const BitSet& other) {
            RM_ASSERT_MSG(words() == other.words(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < words(); i++) {
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
            RM_ASSERT_MSG(words() == other.words(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < words(); i++) {
                at(i) &= other.at(i);
            }
            return *this;
        }

        /**
         * @brief Performs an in‑place AND NOT with another bitset of identical capacity.
         * @param other The bitset to AND NOT with.
         * @return This.
         * @note This operation is analogous to A \ B
         */
        BitSet& operator-=(const BitSet& other) {
            RM_ASSERT_MSG(words() == other.words(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < words(); i++) {
                at(i) &= ~other.at(i);
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
         * @brief Returns the bitwise AND NOT of two bitsets with identical capacity.
         * @param left First bitset.
         * @param right Other bitset.
         * @return A new bitset containing "left AND NOT right".
         */
        friend BitSet operator-(BitSet left, const BitSet& right) { return left -= right; }

        /**
         * @brief Tests whether a specific bit is set.
         * @param bit The bit index to test.
         * @return True if the bit is set, false otherwise.
         */
        b8 test(Alias bit) const noexcept { return (*locate(static_cast<u64>(bit)) & (1ull << (static_cast<u64>(bit) & 63))) != 0; }

        /**
         * @brief Sets a bit to true.
         * @param bit The bit index to set.
         */
        void set(Alias bit) { mutate(static_cast<u64>(bit), true); }

        /**
         * @brief Sets a bit to false.
         * @param bit The bit index to clear.
         */
        void reset(Alias bit) { mutate(static_cast<u64>(bit), false); }

        /**
         * @brief Toggles a bit.
         * @param bit The bit index to toggle.
         */
        void flip(Alias bit) {
            u64* w = locate(static_cast<u64>(bit));
            *w ^= 1ull << (static_cast<u64>(bit) & 63);
        }

        /**
         * @brief Sets all bits to false.
         */
        void clear() noexcept {
            direct.fill(0);
            std::ranges::fill(spill, 0);
        }

        /**
         * @brief Resizes the bitset to accommodate a new number of bits.
         * @param newSize The new size in bits. Should be greater than the current size.
         */
        void resize(u64 newSize) {
            if (newSize <= Size) return;
            spill.resize((newSize - Size + 63) / 64, 0);
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
            if (words() != other.words()) return false;

            for (u64 i = 0; i < words(); i++) {
                if (at(i) & other.at(i)) return true;
            }
            return false;
        }

        private:
        std::array<u64, Size / 64> direct{};  ///< Stack storage for the first Size bits.
        std::vector<u64> spill;               ///< Dynamic storage for bits beyond Size.

        /**
         * @brief Calculates the current storage footprint in 64‑bit words.
         * @return The number of words currently owned (stack + spill).
         */
        u64 words() const noexcept { return direct.max_size() + spill.size(); }

        /**
         * @brief Returns a reference to the storage word at index.
         * @param index The index of the word to access.
         * @return A reference to the storage word at index.
         */
        u64& at(u64 index) noexcept { return index < direct.max_size() ? direct[index] : spill[index - direct.max_size()]; }
        /**
         * @brief Returns a reference to the storage word at index.
         * @param index The index of the word to access.
         * @return A reference to the storage word at index.
         */
        const u64& at(u64 index) const noexcept { return index < direct.max_size() ? direct[index] : spill[index - direct.max_size()]; }

        /**
         * @brief Locates the underlying 64‑bit word that contains a given bit (mutable).
         * @param bit The global bit index to locate.
         * @return A pair of (container pointer, word index) where the bit resides.
         */
        u64* locate(u64 bit) noexcept {
            if (bit < Size) return &direct[bit >> 6];

            const u64 word = (bit - Size) >> 6;
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
        const u64* locate(u64 bit) const noexcept {
            if (bit < Size) return &direct[bit >> 6];

            const u64 word = (bit - Size) >> 6;
            return &spill[word];
        }

        /**
         * @brief Sets or clears a single bit without bounds checking.
         * @param bit The bit index to modify.
         * @param value True to set the bit, false to clear it.
         */
        void mutate(u64 bit, b8 value) {
            u64* w = locate(bit);
            value ? (*w |= 1ull << (bit & 63)) : (*w &= ~(1ull << (bit & 63)));
        }
    };
}  // namespace rome::core