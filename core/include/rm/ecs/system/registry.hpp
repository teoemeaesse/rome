#pragma once

#include <shared_mutex>

#include "rm/ecs/group/group.hpp"
#include "rm/ecs/system/descriptor.hpp"

namespace rome::core {
    namespace System {
        using ID = u32;
        inline constexpr ID INVALID_ID = 0;

        class Registry final {
            public:
            Registry() = default;
            ~Registry() = default;
            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;
            Registry(Registry&&) = delete;
            Registry& operator=(Registry&&) = delete;

            /**
             * @brief Submits a system to the registry.
             * @param The system descriptor.
             * @return True if the system does not exist in the registry, false otherwise.
             */
            b8 submit(Descriptor&& descriptor);

            /**
             * @brief Revokes a system from the registry by name.
             * @param name The name of the system.
             * @return True if the system exists in the registry, false otherwise.
             */
            b8 revoke(std::string_view name);

            /**
             * @brief Revokes a system from the registry by ID.
             * @param id The ID of a system.
             * @return True if the system exists in the registry, false otherwise.
             */
            b8 revoke(ID id);

            /**
             * @brief Checks if a system exists in the registry by name.
             * @param name The name of a system.
             * @return True if the system exists in the registry, false otherwise.
             */
            [[nodiscard]] b8 check(std::string_view name) const noexcept;

            /**
             * @brief Checks if a system exists in the registry by ID.
             * @param id The ID of a system.
             * @return True if the system exists in the registry, false otherwise.
             */
            [[nodiscard]] b8 check(ID id) const noexcept;

            /**
             * @brief Retrieves a system ID by name.
             * @param name The name of a system.
             * @return The system ID, or INVALID_ID if not found.
             */
            [[nodiscard]] ID getID(std::string_view name) const noexcept;

            /**
             * @brief Retrieves a system descriptor by name.
             * @param name The name of a system.
             * @return A system descriptor.
             * @throws Exception::Type::NotFound if not found.
             */
            [[nodiscard]] Descriptor& get(std::string_view name);

            /**
             * @brief Retrieves a const system descriptor by name.
             * @param name The name of a system.
             * @return A const system descriptor.
             * @throws Exception::Type::NotFound if not found.
             */
            [[nodiscard]] const Descriptor& get(std::string_view name) const;

            /**
             * @brief Retrieves a system descriptor by ID.
             * @param id The ID of a system.
             * @return The system descriptor.
             * @throws Exception::Type::NotFound if not found.
             */
            [[nodiscard]] Descriptor& get(ID id);

            /**
             * @brief Retrieves a const system descriptor by ID.
             * @param id The ID of a system.
             * @return The const system descriptor.
             * @throws Exception::Type::NotFound if not found.
             */
            [[nodiscard]] const Descriptor& get(ID id) const;

            /**
             * @brief Retrieves the group owned by a system by name.
             * @param name The name of a system.
             * @return The system's group.
             * @throws Exception::Type::NotFound if not found.
             */
            Group& getGroup(std::string_view name);

            /**
             * @brief Retrieves the group owned by a system by name.
             * @param name The name of a system.
             * @return The system's group.
             * @throws Exception::Type::NotFound if not found.
             */
            const Group& getGroup(std::string_view name) const;

            /**
             * @brief Retrieves the group owned by a system by ID.
             * @param id The ID of a system.
             * @return The system's group.
             * @throws Exception::Type::NotFound if not found.
             */
            Group& getGroup(ID id);

            /**
             * @brief Retrieves the group owned by a system by ID.
             * @param id The ID of a system.
             * @return The system's group.
             * @throws Exception::Type::NotFound if not found.
             */
            const Group& getGroup(ID id) const;

            /**
             * @brief Updates an entity's membership in every submitted group.
             * @param entity The entity whose component archetype changed.
             */
            void updateEntity(Entity entity);

            /**
             * @brief Removes an entity from every submitted group.
             * @param entity The entity to remove.
             */
            void removeEntity(Entity entity);

            /**
             * @brief Executes an active system against its group.
             * @param id The ID of the system to run.
             * @warning This function is not thread-safe.
             * @throws Exception::Type::NotFound if not found.
             */
            void run(ID id);

            /**
             * @brief Executes every active system against its group.
             * @warning This function is not thread-safe.
             */
            void run();
            // TODO: System scheduler for system concurrency

            private:
            std::unordered_map<ID, Group> groups;                                         ///< The groups managed by this registry.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps system names to their IDs.
            std::unordered_map<ID, std::string> names;                                    ///< Reverse lookup.
            std::unordered_map<ID, Descriptor> descriptors;                               ///< Maps system IDs to their descriptors.
            std::queue<ID> freeIDs;                                                       ///< Queue of free IDs for reuse.
            ID nextId = 1;                                                                ///< The next available non-null system ID.
        };
    }  // namespace System
}  // namespace rome::core
