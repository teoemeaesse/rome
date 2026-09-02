#pragma once

#include "rm/prelude.hpp"

namespace rome::core {
    class Filesystem final {
        public:
        Filesystem() = default;
        ~Filesystem() = default;
        Filesystem(const Filesystem&) = delete;
        Filesystem(Filesystem&&) = default;
        Filesystem& operator=(const Filesystem&) = delete;
        Filesystem& operator=(Filesystem&&) = default;

        private:
    };
}  // namespace rome::core
