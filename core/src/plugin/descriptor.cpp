#include "rm/plugin/descriptor.hpp"

#include <utility>

namespace rome::core {
    namespace Plugin {
        Builder::Builder(const std::string_view path) : descriptor{std::string(path)} {}

        Descriptor&& Builder::build() { return std::move(descriptor); }
    }  // namespace Plugin
}  // namespace rome::core
