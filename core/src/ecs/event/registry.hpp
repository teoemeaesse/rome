#pragma once

#include <shared_mutex>

#include "ecs/event/event.hpp"

namespace rome::core {
    namespace Event {
        /**
         * @brief A thread-safe registry for managing events and their unique runtime IDs.
         */
        class Registry {
            public:
            Registry();

            /**
             * @brief Enters a new event into the registry.
             * @param name The name of the event.
             * @return The unique ID of the event.
             */
            ID enter(const std::string& name);

            /**
             * @brief Retrieves the unique ID of an event by its name.
             * @param name The name of the event.
             * @return The unique ID of the event.
             * @throws Exception::Type::NotFound if the event does not exist.
             */
            ID get(const std::string& name) const;

            private:
            mutable std::shared_mutex eventsLock;       ///< Mutex to protect the events map.
            std::unordered_map<std::string, ID> ids;    ///< Maps event names to their unique runtime IDs.
            std::unordered_map<ID, std::string> names;  ///< Reverse lookup.
            std::queue<ID> freeIDs;                     ///< Queue of free IDs for reuse.
        };
    }  // namespace Event
}  // namespace rome::core