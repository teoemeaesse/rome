#pragma once

#include <shared_mutex>

#include "rm/ecs/system/descriptor.hpp"
#include "rm/ecs/group/group.hpp"

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
             * @brief Registers a new system with the given descriptor.
             * @return The ID of the newly registered system, or INVALID_ID if registration fails.
             * @note This function is thread-safe.
             */
            ID enter(Descriptor&& descriptor);

            /**
             * @brief Checks whether a system with the given ID exists.
             * @return True if the system exists, false otherwise.
             * @warning This function is not thread-safe.
             */
            b8 contains(ID id) const noexcept;

            /**
             * @brief Retrieves a mutable reference to a system descriptor.
             * @param id The ID of the system.
             * @return A mutable reference to the system descriptor.
             * @warning This function is not thread-safe.
             * @throws Exception::Type::NotFound if the ID is not registered.
             */
            Descriptor& get(ID id);

            /**
             * @brief Retrieves a const reference to a system descriptor.
             * @throws Exception::Type::NotFound if the ID is not registered.
             * @return A const reference to the system descriptor.
             * @warning This function is not thread-safe.
             * @throws Exception::Type::NotFound if the ID is not registered.
             */
            const Descriptor& get(ID id) const;

            /**
             * @brief Retrieves a mutable reference to the group owned by a system.
             * @param id The ID of the system.
             * @return A mutable reference to the system group.
             * @warning This function is not thread-safe.
             * @throws Exception::Type::NotFound if the ID is not registered.
             */
            Group& getGroup(ID id);

            /**
             * @brief Retrieves a const reference to the group owned by a system.
             * @param id The ID of the system.
             * @return A const reference to the system group.
             * @warning This function is not thread-safe.
             * @throws Exception::Type::NotFound if the ID is not registered.
             */
            const Group& getGroup(ID id) const;

            /**
             * @brief Updates one entity's membership in every registered group.
             * @param entity The entity whose component archetype changed.
             * @note This function is thread-safe.
             */
            void updateEntity(const Entity& entity);

            /**
             * @brief Removes an entity from every registered group.
             * @param entity The entity to remove.
             * @note This function is thread-safe.
             */
            void removeEntity(const Entity& entity);

            /**
             * @brief Executes one active system against its group.
             * @param id The ID of the system to run.
             * @warning This function is not thread-safe.
             * @throws Exception::Type::NotFound if the ID is not registered.
             */
            void run(ID id);

            /**
             * @brief Executes every active system against its group.
             * @warning This function is not thread-safe.
             */
            void run();

            /**
             * @brief Removes a system by ID. Its ID becomes reusable.
             * @param id The ID of the system to remove.
             * @return True if the system was found and removed, false otherwise.
             * @note This function is thread-safe.
             */
            b8 erase(ID id);

            private:
            mutable std::shared_mutex systemsLock;                                        ///< Mutex for thread-safe access.
            std::unordered_map<ID, Group> groups;                                         ///< The groups managed by this registry.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps system names to their IDs.
            std::unordered_map<ID, const std::string> names;                              ///< Reverse lookup.
            std::unordered_map<ID, Descriptor> descriptors;                               ///< Maps system IDs to their descriptors.
            std::queue<ID> freeIDs;                                                       ///< Queue of free IDs for reuse.
            ID nextId = 1;                                                                ///< The next available non-null system ID.
        };
    }  // namespace System
}  // namespace rome::core
