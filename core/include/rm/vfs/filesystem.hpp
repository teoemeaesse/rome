#pragma once

#include "rm/container/sparse_set.hpp"
#include "rm/vfs/inode.hpp"

namespace rome::core {
    class Filesystem final {
        public:
        Filesystem() = default;
        ~Filesystem();
        Filesystem(const Filesystem&) = delete;
        Filesystem(Filesystem&&) = default;
        Filesystem& operator=(const Filesystem&) = delete;
        Filesystem& operator=(Filesystem&&) = default;

        private:
        SparseSet<Inode> inodes;  ///< The Inode list for the entire filesystem.
    };
}  // namespace rome::core
