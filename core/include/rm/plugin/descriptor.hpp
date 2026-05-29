#pragma once

#include <string>
#include <string_view>

#include "rm/plugin/plugin.hpp"

namespace rome::core {
    namespace Plugin {
        /**
         * @brief A plugin descriptor defines the dynamic library to load.
         */
        struct Descriptor {
            std::string path = "null descriptor";  ///< The plugin library path. Must be unique once resolved.
        };

        class Builder {
            public:
            Builder(const std::string_view path);
            ~Builder() = default;
            Builder(const Builder&) = delete;
            Builder& operator=(const Builder&) = delete;
            Builder(Builder&&) = delete;
            Builder& operator=(Builder&&) = delete;

            /**
             * @brief Creates a ready-to-submit plugin descriptor.
             * @return A fully-populated descriptor.
             */
            Descriptor&& build();

            private:
            Descriptor descriptor;  ///< The plugin descriptor being built.
        };
    }  // namespace Plugin
}  // namespace rome::core
