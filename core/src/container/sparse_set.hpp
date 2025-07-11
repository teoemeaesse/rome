#pragma once

#include "debug/exception.hpp"

namespace rome::core {
    template <typename T>
    class RM_API SparseSet final {
        public:
        SparseSet() : size(0) {};
        ~SparseSet() = default;
        SparseSet(const SparseSet& other) = default;
        SparseSet(SparseSet&& other) = default;
        SparseSet& operator=(const SparseSet& other) = default;
        SparseSet& operator=(SparseSet&& other) = default;

        /**
         * @brief Copies a value into the sparse set.
         * @param index The index to copy the value to.
         * @param value The value to copy.
         */
        void insert(u64 index, const T& value) {
            if (contains(index)) {
                return;
            }
            if (index >= sparse.size()) {
                sparse.resize(index + 1, 0);
            }
            if (size >= dense.size()) {
                dense.resize(size + 1, 0);
            }
            dense[size] = index;
            sparse[index] = size;
            data.push_back(value);
            size++;
        }

        /**
         * @brief Moves a value into the sparse set.
         * @param index The index to move the value to.
         * @param value The value to move.
         */
        void insert(u64 index, T&& value) {
            if (contains(index)) {
                return;
            }
            if (index >= sparse.size()) {
                sparse.resize(index + 1, 0);
            }
            if (size >= dense.size()) {
                dense.resize(size + 1, 0);
            }
            dense[size] = index;
            sparse[index] = size;
            data.emplace_back(std::move(value));
            size++;
        }

        /**
         * @brief Builds a value in place in the sparse set.
         * @tparam ...Args The types of the arguments to forward to the value constructor.
         * @param index The index to build the value at.
         * @param ...args The arguments to forward to the value constructor.
         */
        template <typename... Args>
        void emplace(u64 index, Args&&... args) {
            if (contains(index)) {
                return;
            }
            if (index >= sparse.size()) {
                sparse.resize(index + 1, 0);
            }
            if (size >= dense.size()) {
                dense.resize(size + 1, 0);
            }
            dense[size] = index;
            sparse[index] = size;
            data.emplace_back(std::forward<Args>(args)...);
            size++;
        }

        /**
         * @brief Removes an element from the sparse set.
         * @param index The index of the element to remove.
         */
        void erase(u64 index) {
            if (!contains(index)) {
                return;
            }
            sparse[dense[size - 1]] = sparse[index];
            std::swap(dense[sparse[index]], dense[size - 1]);
            std::swap(data[sparse[index]], data[size - 1]);
            dense.pop_back();
            data.pop_back();
            size--;
        }

        /**
         * @brief Swaps two elements in the sparse set.
         * @param index1 The index of the first element to swap.
         * @param index2 The index of the second element to swap.
         */
        void swap(u64 index1, u64 index2) {
            if (index1 == index2 || !contains(index1) || !contains(index2)) {
                return;
            }
            u64 pos1 = sparse[index1];
            u64 pos2 = sparse[index2];

            std::swap(dense[pos1], dense[pos2]);
            std::swap(sparse[index1], sparse[index2]);
            std::swap(data[pos1], data[pos2]);
        }

        /**
         * @brief Fetches the data of the sparse set.
         * @return A pair containing a pointer to the data and the size of the sparse set.
         * @warning The data pointer is only valid as long as the sparse set's size does not change.
         */
        std::pair<T*, u64> getData() { return {data.data(), size}; }

        const T& operator[](u64 index) const {
            if (!contains(index)) {
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Sparse set does not contain value at index");
            }
            return data[sparse[index]];
        }

        T& operator[](u64 index) {
            if (!contains(index)) {
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Sparse set does not contain value at index");
            }
            return data[sparse[index]];
        }

        /**
         * @brief Gets the value at the given index.
         * @param index The index to get the value from.
         * @return The value at the given index.
         */
        const T& at(u64 index) const {
            if (!contains(index)) {
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Sparse set does not contain value at index");
            }
            return data[sparse[index]];
        }

        /**
         * @brief Gets the value at the given index.
         * @param index The index to get the value from.
         * @return The value at the given index.
         */
        T& at(u64 index) {
            if (!contains(index)) {
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, "Sparse set does not contain value at index");
            }
            return data[sparse[index]];
        }

        /**
         * @brief Checks if the sparse set contains a value at the given index.
         * @param index The index to check.
         * @return True if the sparse set contains a value at the given index, false otherwise.
         */
        inline b8 contains(u64 index) const noexcept { return index < sparse.size() && sparse[index] < size && dense[sparse[index]] == index; }

        inline u64 getSize() const noexcept { return size; }

        /* Non-const iterator interfaces */
        inline std::vector<T>::iterator begin() { return data.begin(); }
        inline std::vector<T>::iterator end() { return data.begin() + size; }

        /* Const iterator interfaces */
        inline std::vector<T>::const_iterator begin() const { return data.begin(); }
        inline std::vector<T>::const_iterator end() const { return data.begin() + size; }

        private:
        std::vector<u64> dense;   ///< Maps dense index to sparse index
        std::vector<u64> sparse;  ///< Maps sparse index to dense index
        std::vector<T> data;      ///< Data storage
        u64 size;                 ///< Number of elements in the sparse set
    };
}  // namespace rome::core