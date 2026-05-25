#pragma once

#include <shared_mutex>

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
            ~Registry();
            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;
            Registry(Registry&&) = delete;
            Registry& operator=(Registry&&) = delete;

            /**
             * @brief Loads a plugin and runs its load entry point.
             * @param path The plugin library path.
             * @param ecs The ECS instance exposed to the plugin.
             * @return The loaded plugin ID, or Plugin::INVALID_ID.
             */
            ID load(const std::string& path, ECS& ecs);

            /**
             * @brief Unloads a plugin.
             * @param id The loaded plugin ID.
             * @param ecs The ECS instance exposed to the plugin.
             * @return True if the plugin existed and was unloaded.
             */
            b8 unload(ID id, ECS& ecs);

            /**
             * @brief Unloads every loaded plugin.
             * @param ecs The ECS instance exposed to each plugin.
             */
            void unload(ECS& ecs);

            /**
             * @brief Gets the number of loaded plugins.
             * @return The number of loaded plugins.
             */
            u32 getSize() const noexcept;

            private:
            std::string resolvePath(const std::string& path) const;

            mutable std::shared_mutex pluginsLock;  ///< Guards loaded library storage.
            std::unordered_map<ID, Library> libraries;  ///< Maps plugin IDs to loaded libraries.
            ID nextId = 1;  ///< Next plugin ID to assign.
        };
    }  // namespace Plugin
}  // namespace rome::core
