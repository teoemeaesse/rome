#pragma once

#include <limits>

#include "rm/debug/exception.hpp"
#include "rm/debug/log.hpp"

namespace rome::core {
    template <typename T>
    class SparseSet final {
        public:
        SparseSet() : size(0) {};
        ~SparseSet() = default;
        SparseSet(const SparseSet& other) = default;
        SparseSet(SparseSet&& other) noexcept = default;
        SparseSet& operator=(const SparseSet& other) = default;
        SparseSet& operator=(SparseSet&& other) noexcept = default;

        /**
         * @brief Copies a value into the sparse set. Fails silently if inserting would overwrite existing data.
         * @param index The index to copy the value to.
         * @param value The value to copy.
         * @return True if the insert succeeded, false otherwise.
         */
        b8 try_insert(u64 index, const T& value) {
            if (index == std::numeric_limits<u64>::max()) {
                RM_DEBUG("Index is out of bounds");
                return false;
            }
            if (contains(index)) {
                RM_DEBUG("Tried to insert duplicate index");
                return false;
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
            return true;
        }

        /**
         * @brief Moves a value into the sparse set. Fails silently if inserting would overwrite existing data.
         * @param index The index to move the value to.
         * @param value The value to move.
         * @return True if the insert succeeded, false otherwise.
         */
        b8 try_insert(u64 index, T&& value) {
            if (index < 0 || index >= std::numeric_limits<u64>::max()) {
                RM_DEBUG("Index is out of bounds");
                return false;
            }
            if (contains(index)) {
                RM_DEBUG("Tried to insert duplicate index");
                return false;
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
            return true;
        }

        /**
         * @brief Copies a value into the sparse set.
         * @param index The index to copy the value to.
         * @param value The value to copy.
         */
        void insert(u64 index, const T& value) {
            if (index == std::numeric_limits<u64>::max()) {
                RM_DEBUG("Index is out of bounds");
                return;
            }
            if (contains(index)) {
                data[sparse[index]] = value;
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
            if (index == std::numeric_limits<u64>::max()) {
                RM_DEBUG("Index is out of bounds");
                return;
            }
            if (contains(index)) {
                data[sparse[index]] = std::move(value);
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
         * @brief Swaps the internal position of two elements.
         * @param index1 The index of the first element to swap.
         * @param index2 The index of the second element to swap.
         * @warning Invalidates all iterators.
         */
        void swap(u64 index1, u64 index2) noexcept {
            if (index1 == index2 || !contains(index1) || !contains(index2)) {
                RM_DEBUG("Tried to swap invalid indices");
                return;
            }
            u64 pos1 = sparse[index1];
            u64 pos2 = sparse[index2];

            std::swap(dense[pos1], dense[pos2]);
            std::swap(data[pos1], data[pos2]);

            sparse[dense[pos1]] = pos1;
            sparse[dense[pos2]] = pos2;
        }

        /**
         * @brief Fetches the data of the sparse set.
         * @return A pair containing a pointer to the data and the size of the sparse set.
         * @warning The data pointer is only valid as long as the sparse set's size does not change.
         */
        std::pair<T*, u64> getData() noexcept { return {data.data(), size}; }

        /**
         * @brief Fetches the data of the sparse set.
         * @return A pair containing a pointer to the data and the size of the sparse set.
         * @warning The data pointer is only valid as long as the sparse set's size does not change.
         */
        std::pair<const T*, u64> getData() const noexcept { return {data.data(), size}; }

        /**
         * @brief Gets a pointer to the given index.
         * @param index The index to get the value from.
         * @return The pointer to the given index.
         */
        T* operator[](u64 index) noexcept {
            if (!contains(index)) {
                return nullptr;
            }
            return data.data() + sparse[index];
        }

        /**
         * @brief Gets a pointer to the given index.
         * @param index The index to get the value from.
         * @return The pointer to the given index.
         */
        const T* operator[](u64 index) const noexcept {
            if (!contains(index)) {
                return nullptr;
            }
            return data.data() + sparse[index];
        }

        /**
         * @brief Gets the value at the given index.
         * @param index The index to get the value from.
         * @return The value at the given index.
         * @throws Exception::Type::NotFound if the value was not found.
         */
        const T& at(u64 index) const {
            if (!contains(index)) {
                std::string msg = "The index " + std::to_string(index) + " is out of bounds";
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
            }
            return data[sparse[index]];
        }

        /**
         * @brief Gets the value at the given index.
         * @param index The index to get the value from.
         * @return The value at the given index.
         * @throws Exception::Type::NotFound if the value was not found.
         */
        T& at(u64 index) {
            if (!contains(index)) {
                std::string msg = "The index " + std::to_string(index) + " is out of bounds";
                THROW_CORE_EXCEPTION(Exception::Type::NotFound, msg.c_str());
            }
            return data[sparse[index]];
        }

        /**
         * @brief Checks if the sparse set contains a value at the given index.
         * @param index The index to check.
         * @return True if the sparse set contains a value at the given index, false otherwise.
         */
        b8 contains(u64 index) const noexcept { return index < sparse.size() && sparse[index] < size && dense[sparse[index]] == index; }

        /**
         * @brief Returns the number of elements in the sparse set.
         * @return The number of elements in the sparse set.
         */
        u64 getSize() const noexcept { return size; }

        /* Non-const iterator interfaces */
        inline std::vector<T>::iterator begin() noexcept { return data.begin(); }
        inline std::vector<T>::iterator end() noexcept { return data.end(); }
        inline std::vector<T>::iterator find(u64 index) noexcept {
            if (!contains(index)) return end();
            return begin() + sparse[index];
        }

        /* Const iterator interfaces */
        inline std::vector<T>::const_iterator begin() const noexcept { return data.begin(); }
        inline std::vector<T>::const_iterator end() const noexcept { return data.end(); }
        inline std::vector<T>::const_iterator find(u64 index) const noexcept {
            if (!contains(index)) return end();
            return begin() + sparse[index];
        }

        private:
        std::vector<u64> dense;   ///< Maps dense index to sparse index.
        std::vector<u64> sparse;  ///< Maps sparse index to dense index.
        std::vector<T> data;      ///< Data storage.
        u64 size;                 ///< Number of elements in the sparse set.
    };
}  // namespace rome::core
