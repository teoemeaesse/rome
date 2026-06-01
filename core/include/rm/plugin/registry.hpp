#pragma once

#include <shared_mutex>

#include "rm/plugin/descriptor.hpp"
#include "rm/plugin/library.hpp"
#include "rm/plugin/plugin.hpp"

namespace rome::core {
    namespace Plugin {
        /**
         * @brief Owns dynamically loaded plugins.
         */
        class Registry final {
            public:
            Registry() = default;
            ~Registry() = default;
            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;
            Registry(Registry&&) = delete;
            Registry& operator=(Registry&&) = delete;

            /**
             * @brief Loads a plugin dynamic library and runs its load entry point.
             * @param descriptor The plugin descriptor.
             * @param ecs The ECS instance exposed to the plugin.
             * @return True if the plugin loaded or was already loaded, false otherwise.
             */
            b8 load(Descriptor&& descriptor, ECS& ecs);

            /**
             * @brief Unloads a plugin dynamic library and runs its unload entry point.
             * @param id The plugin ID.
             * @param ecs The ECS instance exposed to the plugin.
             * @return True if the plugin exists in the registry, false otherwise.
             */
            b8 unload(ID id, ECS& ecs);

            /**
             * @brief Unloads a plugin dynamic library by path and runs its unload entry point.
             * @param path The plugin library path.
             * @param ecs The ECS instance exposed to the plugin.
             * @return True if the plugin exists in the registry, false otherwise.
             */
            b8 unload(const std::string_view path, ECS& ecs);

            /**
             * @brief Checks whether a plugin declaration with the given ID exists.
             * @param id The plugin ID.
             * @return True if the plugin exists in the registry, false otherwise.
             */
            b8 check(ID id) const noexcept;

            /**
             * @brief Checks whether a plugin declaration with the given path exists.
             * @param path The plugin library path.
             * @return True if the plugin exists in the registry, false otherwise.
             */
            b8 check(const std::string_view path) const;

            /**
             * @brief Unloads every loaded plugin dynamic library.
             * @param ecs The ECS instance exposed to each plugin.
             */
            void unloadAll(ECS& ecs);

            /**
             * @brief Gets a loaded plugin ID by path.
             * @param path The plugin library path.
             * @return The plugin ID, or Plugin::INVALID_ID if not found.
             */
            ID get(const std::string_view path) const;

            /**
             * @brief Gets the number of plugins in the registry.
             * @return The number of plugins.
             */
            u32 getSize() const noexcept;

            private:
            mutable std::shared_mutex pluginsLock;      ///< Guards loaded library storage.
            std::unordered_map<ID, Library> libraries;  ///< Maps plugin IDs to loaded libraries.
            ID nextId = 1;                              ///< Next plugin ID to assign.

            std::string resolvePath(const std::string_view path) const;
        };
    }  // namespace Plugin
}  // namespace rome::core
