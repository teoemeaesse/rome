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

        private:
        std::unordered_map<std::string, u64, TransparentSVHash, std::equal_to<>> mappings;  ///< Maps item names to Inode ID's.
    };
}  // namespace rome::core
