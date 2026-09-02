#include "rm/vfs/item.hpp"

namespace rome::core {
    Item::Type Item::getType() {
        if (table.size() == 0) return Type::File;
        return Type::Directory;
    }
}  // namespace rome::core
