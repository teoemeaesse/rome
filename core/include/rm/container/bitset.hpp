#pragma once

#include "rm/debug/log.hpp"
#include "rm/prelude.hpp"

namespace rome::core {

    /**
     * @brief A flexible bitset that can grow beyond a fixed size.
     * This bitset stores the first 512 bits (8 words) in the stack and spills over to a vector for additional bits.
     * @tparam Size The maximum expected capacity in bits (default is 512 bits).
     *              You can go over this size, falling back to dynamic storage.
     */
    template <u64 Size = 512>
    class BitSet final {
        STATIC_ASSERT(Size % 64 == 0, "Size must be a multiple of 64 bits");

        public:
        BitSet() = default;
        explicit BitSet(u64 size) { resize(size); }
        BitSet(std::initializer_list<u64> bits) {
            for (u64 bit : bits) {
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
        template <typename Word>
        static BitSet create(std::initializer_list<Word> bits) {
            STATIC_ASSERT(std::is_unsigned_v<Word>, "Word must be an unsigned integer type");
            BitSet set(Size);
            for (Word bit : bits) {
                set.set(bit);
            }
            return set;
        }

        /**
         * @brief Performs an in‑place OR with another bitset of identical capacity.
         * @param other The bitset to OR with.
         * @return This after "this OR other".
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
         * @return This after "this AND other".
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
         * @return This after "this AND NOT other".
         * @note This operation is analogous to "this \ other".
         */
        BitSet& operator-=(const BitSet& other) {
            RM_ASSERT_MSG(words() == other.words(), "Bitset sizes differ — resize all masks first");

            for (u64 i = 0; i < words(); i++) {
                at(i) &= ~other.at(i);
            }
            return *this;
        }

        /**
         * @brief Returns whether two bitsets with identical capacity are exactly equal.
         * @param left Left operand.
         * @param right Right operand.
         * @return True if the bitsets are exactly equal, false otherwise.
         */
        friend b8 operator==(const BitSet& left, const BitSet& right) {
            RM_ASSERT_MSG(left.words() == right.words(), "Bitset sizes differ — resize all masks first");

            b8 isEqual = true;
            for (u64 i = 0; i < left.words(); i++) {
                if (left.at(i) != right.at(i)) {
                    isEqual = false;
                    break;
                };
            }
            return isEqual;
        }

        /**
         * @brief Returns the bitwise OR of two bitsets with identical capacity.
         * @param left Left operand.
         * @param right Right operand.
         * @return A new bitset containing "left OR right".
         */
        friend BitSet operator|(const BitSet& left, const BitSet& right) {
            RM_ASSERT_MSG(left.words() == right.words(), "Bitset sizes differ — resize all masks first");

            BitSet out = left;
            for (u64 i = 0; i < left.words(); i++) {
                out.at(i) |= right.at(i);
            }
            return out;
        }

        /**
         * @brief Returns the bitwise AND of two bitsets with identical capacity.
         * @param left Left operand.
         * @param right Right operand.
         * @return A new bitset containing "left AND right".
         */
        friend BitSet operator&(const BitSet& left, const BitSet& right) {
            RM_ASSERT_MSG(left.words() == right.words(), "Bitset sizes differ — resize all masks first");

            BitSet out = left;
            for (u64 i = 0; i < left.words(); i++) {
                out.at(i) &= right.at(i);
            }
            return out;
        }

        /**
         * @brief Returns the bitwise AND NOT of two bitsets with identical capacity.
         * @param left First operand.
         * @param right Right operand.
         * @return A new bitset containing "left AND NOT right".
         * @note This operation is analogous to "left \ right".
         */
        friend BitSet operator-(const BitSet& left, const BitSet& right) {
            RM_ASSERT_MSG(left.words() == right.words(), "Bitset sizes differ — resize all masks first");

            BitSet out = left;
            for (u64 i = 0; i < left.words(); i++) {
                out.at(i) &= ~right.at(i);
            }
            return out;
        }

        /**
         * @brief Tests whether a specific bit is set.
         * @param bit The bit index to test.
         * @return True if the bit is set, false otherwise.
         */
        b8 test(u64 bit) const {
            const u64* loc = locate(bit);
            if (loc == nullptr) return false;
            return (*loc & (1ull << (bit & 63))) != 0;
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
        void flip(u64 bit) {
            u64* w = locate(bit);
            *w ^= 1ull << (bit & 63);
        }

        /**
         * @brief Sets all bits to 0.
         */
        void clear() {
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
        b8 any() const { return !none(); }

        /**
         * @brief Checks whether no bits are set.
         * @return True if no bits are set, false if at least one bit is set.
         */
        b8 none() const {
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
        u64 count() const {
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
        b8 intersects(const BitSet& other) const {
            if (words() != other.words()) return false;

            for (u64 i = 0; i < words(); i++) {
                if (at(i) & other.at(i)) return true;
            }
            return false;
        }

        /**
         * @brief Calculates the current storage footprint in 64‑bit words.
         * @return The number of words currently owned (stack + spill).
         */
        u64 words() const { return direct.max_size() + spill.size(); }

        /**
         * @brief Returns a reference to the storage word at index.
         * @param index The index of the word to access.
         * @return A reference to the storage word at index.
         */
        u64& at(u64 index) { return index < direct.max_size() ? direct[index] : spill[index - direct.max_size()]; }
        /**
         * @brief Returns a reference to the storage word at index.
         * @param index The index of the word to access.
         * @return A reference to the storage word at index.
         */
        const u64& at(u64 index) const { return index < direct.max_size() ? direct[index] : spill[index - direct.max_size()]; }

        private:
        std::array<u64, Size / 64> direct{};  ///< Stack storage for the first Size bits.
        std::vector<u64> spill;               ///< Dynamic storage for bits beyond Size.

        /**
         * @brief Locates the underlying 64‑bit word that contains a given bit (mutable).
         * @param bit The global bit index to locate.
         * @return A pointer to where the bit resides.
         */
        u64* locate(u64 bit) {
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
         * @return A pointer to where the bit resides.
         */
        const u64* locate(u64 bit) const {
            if (bit < Size) return &direct[bit >> 6];

            const u64 word = (bit - Size) >> 6;
            if (spill.size() <= word) {
                RM_DEBUG("Bit is out of bounds");
                return nullptr;
            }
            return &spill[word];
        }

        /**
         * @brief Sets or clears a single bit.
         * @param bit The bit index to modify.
         * @param value True to set the bit, false to clear it.
         */
        void mutate(u64 bit, b8 value) {
            u64* w = locate(bit);
            value ? (*w |= 1ull << (bit & 63)) : (*w &= ~(1ull << (bit & 63)));
        }
    };
}  // namespace rome::core
