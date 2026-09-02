#pragma once

#include "rm/vfs/inode.hpp"

namespace rome::core {
    class Item {
        public:
        Item() = default;
        ~Item() = default;
        Item(const Item&) = default;
        Item(Item&&) = default;
        Item& operator=(const Item&) = default;
        Item& operator=(Item&&) = default;

        enum class Type {
            File = 0,
            Directory,
        };

        /**
         * @brief Checks the concrete type of this item.
         * @return The concrete type of this item.
         */
        Type getType();

        private:
        std::unordered_map<std::string, Inode, TransparentSVHash, std::equal_to<>> table;
    };
}  // namespace rome::core
