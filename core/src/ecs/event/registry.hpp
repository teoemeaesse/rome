#pragma once

#include <shared_mutex>

#include "ecs/event/event.hpp"

namespace rome::core {
    namespace Event {
        /**
         * @brief A registry for managing events and their unique runtime IDs.
         * @warning This registry is not thread-safe outside event registration.
         */
        class RM_API Registry final {
            public:
            Registry() = default;
            ~Registry() = default;
            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;
            Registry(Registry&&) = delete;
            Registry& operator=(Registry&&) = delete;

            /**
             * @brief Enters a new event into the registry.
             * @param name The name of the event.
             * @return The unique ID of the event.
             * @note This function is thread-safe.
             */
            ID enter(const std::string& name);

            /**
             * @brief Retrieves the unique ID of an event by its name.
             * @param name The name of the event.
             * @return The unique ID of the event.
             * @throws Exception::Type::NotFound if the event does not exist.
             * @warning This function is not thread-safe.
             */
            ID get(const std::string& name) const;

            /**
             * @brief Checks if an event exists in the registry.
             * @param name The name of the event.
             * @return True if the event exists, false otherwise.
             * @warning This function is not thread-safe.
             */
            b8 contains(const std::string& name) const;

            /**
             * @brief Retrieves the unique ID of an event by its type.
             * @tparam E The type of the event.
             * @return The unique ID of the event.
             * @throws Exception::Type::NotFound if the event does not exist.
             * @warning This function is not thread-safe.
             */
            template <Event E>
            ID get() const {
                return get(Reflect::reflect<E>().name);
            }

            private:
            mutable std::shared_mutex eventsLock;                                         ///< Mutex to protect the events map.
            std::unordered_map<std::string, ID, TransparentSVHash, std::equal_to<>> ids;  ///< Maps event names to their IDs.
            std::unordered_map<ID, std::string> names;                                    ///< Reverse lookup.
            std::queue<ID> freeIDs;                                                       ///< Queue of free IDs for reuse.
        };
    }  // namespace Event
}  // namespace rome::core