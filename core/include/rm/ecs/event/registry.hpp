#pragma once

#include "rm/ecs/event/event.hpp"

namespace rome::core {
    namespace Event {
        /**
         * @brief A registry for managing the lifetime of events.
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
             * @brief Submits an event to the registry by name.
             * @param name The name of the event.
             * @return True if the event does not exist in the registry, false otherwise.
             */
            b8 submit(std::string_view name);

            /**
             * @brief Submits a new event to the registry by type.
             * @tparam E The type of the event.
             * @return True if the event does not exist in the registry, false otherwise.
             */
            template <Event E>
            b8 submit() {
                return submit(getName<E>());
            }

            /**
             * @brief Revokes an event from the registry by name.
             * @param name The name of the event.
             * @return True if the event exists in the registry, false otherwise.
             */
            b8 revoke(std::string_view name);

            /**
             * @brief Revokes an event from the registry by type.
             * @tparam E The type of the event.
             * @return True if the event exists in the registry, false otherwise.
             */
            template <Event E>
            b8 revoke() {
                return revoke(getName<E>());
            }

            /**
             * @brief Checks whether an event exists in the registry by name.
             * @param name The name of the event.
             * @return True if the event exists in the registry, false otherwise.
             */
            [[nodiscard]] b8 check(std::string_view name) const;

            /**
             * @brief Checks whether an event exists in the registry by type.
             * @tparam E The type of the event.
             * @return True if the event exists in the registry, false otherwise.
             */
            template <Event E>
            [[nodiscard]] b8 check() const {
                return check(getName<E>());
            }

            /**
             * @brief Retrieves the unique ID of an event by name.
             * @param name The name of an event.
             * @return The unique ID of the event, or INVALID_ID if not found.
             */
            [[nodiscard]] ID getID(std::string_view name) const;

            /**
             * @brief Retrieves the unique ID of an event by type.
             * @tparam E The type of the event.
             * @return The unique ID of the event, or INVALID_ID if not found.
             */
            template <Event E>
            [[nodiscard]] ID getID() const {
                return getID(getName<E>());
            }

            private:
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps event names to their IDs.
            std::unordered_map<ID, std::string> names;                                    ///< Reverse lookup.
            std::queue<ID> freeIDs;                                                       ///< Queue of free IDs for reuse.
            ID nextId = 1;                                                                ///< The next available non-null event ID.

            /**
             * @brief Retrieves the name of an event by its type.
             * @tparam E The type of the event.
             * @return The name of the event.
             * @note The event does not need to exist in the registry.
             */
            template <Event E>
            constexpr const std::string_view getName() const {
                return Reflect::reflect<E>().getType().getName();
            }
        };
    }  // namespace Event
}  // namespace rome::core
