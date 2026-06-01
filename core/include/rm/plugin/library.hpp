#pragma once

#include <string>

#include "rm/plugin/descriptor.hpp"
#include "rm/plugin/plugin.hpp"

namespace rome::core {
    namespace Plugin {
        /**
         * @brief Owns one dynamically loaded plugin library.
         */
        class Library final {
            public:
            Library(ID id, Descriptor&& descriptor, void* handle);
            ~Library();
            Library(const Library& other) = delete;
            Library(Library&& other) noexcept;

            Library& operator=(const Library& other) = delete;
            Library& operator=(Library&& other) noexcept;

            inline ID getID() const noexcept { return id; }
            inline std::string_view getLoadingPath() const noexcept { return descriptor.path; }

            /**
             * @brief Runs the plugin unload hook if one was exported.
             * @param ecs The ECS instance.
             */
            void unload(ECS& ecs);

            private:
            void close() noexcept;

            ID id = INVALID_ID;      ///< The plugin ID assigned to this loaded library.
            Descriptor descriptor;   ///< The loaded plugin descriptor.
            void* handle = nullptr;  ///< The native dynamic library handle.
        };
    }  // namespace Plugin
}  // namespace rome::core
