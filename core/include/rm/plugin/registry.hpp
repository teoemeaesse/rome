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
             * @brief Submits a plugin declaration and runs its load entry point.
             * @param path The plugin library path.
             * @param ecs The ECS instance exposed to the plugin.
             * @return True if the plugin was newly submitted, false otherwise.
             */
            b8 submit(const std::string& path, ECS& ecs);

            /**
             * @brief Revokes a plugin declaration and runs its unload entry point.
             * @param id The submitted plugin ID.
             * @param ecs The ECS instance exposed to the plugin.
             * @return True if the plugin existed and was revoked.
             */
            b8 revoke(ID id, ECS& ecs);

            /**
             * @brief Revokes every submitted plugin declaration.
             * @param ecs The ECS instance exposed to each plugin.
             */
            void revoke(ECS& ecs);

            /**
             * @brief Checks whether a plugin declaration with the given ID exists.
             * @param id The submitted plugin ID.
             * @return True if the plugin exists, false otherwise.
             */
            b8 check(ID id) const noexcept;

            /**
             * @brief Gets a submitted plugin ID by path.
             * @param path The plugin library path.
             * @return The submitted plugin ID, or Plugin::INVALID_ID if the path has not been submitted.
             */
            ID get(const std::string& path) const;

            /**
             * @brief Gets the number of submitted plugins.
             * @return The number of submitted plugins.
             */
            u32 getSize() const noexcept;

            private:
            std::string resolvePath(const std::string& path) const;

            mutable std::shared_mutex pluginsLock;      ///< Guards submitted library storage.
            std::unordered_map<ID, Library> libraries;  ///< Maps plugin IDs to submitted libraries.
            ID nextId = 1;                              ///< Next plugin ID to assign.
        };
    }  // namespace Plugin
}  // namespace rome::core
