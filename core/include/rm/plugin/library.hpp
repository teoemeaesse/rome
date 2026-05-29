#pragma once

#include <string>

#include "rm/plugin/plugin.hpp"

namespace rome::core {
    namespace Plugin {
        /**
         * @brief Owns one dynamically loaded plugin library.
         */
        class Library final {
            public:
            Library(ID id, const std::string_view loadingPath, void* handle, UnloadFn unload);
            ~Library();
            Library(const Library&) = delete;
            Library(Library&& other) noexcept;

            Library& operator=(const Library&) = delete;
            Library& operator=(Library&& other) noexcept;

            inline ID getID() const noexcept { return id; }
            inline std::string_view getLoadingPath() const noexcept { return loadingPath; }
            inline u32 getReferences() const noexcept { return references; }
            inline void addReference() noexcept { references++; }
            inline void removeReference() noexcept { references--; }

            /**
             * @brief Runs the plugin unload hook if one was exported.
             * @param ecs The ECS instance exposed to the plugin.
             */
            void unloadFrom(ECS& ecs);

            private:
            void close() noexcept;

            ID id = INVALID_ID;         ///< The plugin ID assigned to this loaded library.
            std::string loadingPath;    ///< The resolved path used to load this library.
            void* handle = nullptr;     ///< The native dynamic library handle.
            UnloadFn unload = nullptr;  ///< Optional plugin unload entry point.
            u32 references = 1;         ///< Number of active load requests for this library.
        };
    }  // namespace Plugin
}  // namespace rome::core
