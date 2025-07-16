#pragma once

#include "ecs/world.hpp"

namespace rome::core {
    namespace Event {
        /**
         * @brief A base class for event queue storage.
         */
        class RM_API Queue {
            public:
            virtual ~Queue() = default;

            /**
             * @brief Swaps the front (read) and back (write) buffers.
             */
            virtual void swap() = 0;
        };

        /**
         * @brief A storage for every event of this type.
         * @tparam E The event type to store.
         */
        template <Event E>
        class RM_API Storage final : public Queue {
            public:
            Storage() = default;
            ~Storage() = default;
            Storage(const Storage&) = delete;
            Storage& operator=(const Storage&) = delete;
            Storage(Storage&&) = default;
            Storage& operator=(Storage&&) = default;

            /**
             * @brief Adds an event to the queue.
             * @param event The event to queue.
             */
            void push(const E& event) { back.push_back(event); }

            /**
             * @brief Builds an event in-place in the queue.
             * @tparam ...Args The types of arguments to pass to the event constructor.
             * @param ...args The arguments to pass to the event constructor.
             */
            template <typename... Args>
            void emplace(Args&&... args) {
                back.emplace_back(std::forward<Args>(args)...);
            }

            /**
             * @brief Checks whether the queue is empty.
             * @return True if the queue is empty, false otherwise.
             */
            b8 empty() const { return front.empty() && back.empty(); }

            /**
             * @brief Returns a back-to-front view of the queue's front buffer.
             * @return A view of the queue.
             */
            std::span<const E> read() const { return std::span<const E>(front.data(), front.size()); }

            /**
             * @brief Swaps the front (read) and back (write) buffers.
             */
            void swap() override {
                front.swap(back);
                back.clear();
            }

            private:
            std::vector<E> front;  ///< Consumers read from this vector.
            std::vector<E> back;   ///< Producers write to this vector.
        };

        class RM_API Bus final {
            public:
            Bus() = default;
            ~Bus() = default;

            /**
             * @brief Enters a new event queue into the bus.
             * @tparam E The event type to enter.
             * @throws Exception::Type::InvalidArgument if the event queue already exists.
             * @note This function is thread-safe.
             */
            template <Event E>
            void enter() {
                std::unique_lock lock(queuesLock);
                auto it = queues.find(world.events.get<E>());
                if (it != queues.end()) {
                    string msg = "Event queue for '" + Reflect::reflect<E>().name + "' already exists";
                    THROW_CORE_EXCEPTION(Exception::Type::InvalidArgument, msg.);
                }
                queues.emplace(world.events.get<E>(), MakeUnique<Storage<E>>());
            }

            /**
             * @brief Retrieves the event queue for a specific event type.
             * @tparam E The event type to retrieve.
             * @return A reference to the event queue.
             * @throws Exception::Type::InvalidArgument if the event queue does not exist.
             * @warning This function is not thread-safe.
             */
            template <Event E>
            Storage<E>& queue() {
                const ID id = world.events.get<E>();

                auto it = queues.find(id);
                if (it == queues.end()) {
                    std::string msg = "Event queue for '" + Reflect::reflect<E>().name + "' does not exist";
                    THROW_CORE_EXCEPTION(Exception::Type::InvalidArgument, msg.c_str());
                }
                return *static_cast<Storage<E>*>(it->second.get());
            }

            /**
             * @brief Swaps the front (read) and back (write) buffers for all event queues.
             */
            void swap();

            private:
            std::shared_mutex queuesLock;                  ///< Mutex to protect the queues map.
            std::unordered_map<ID, Unique<Queue>> queues;  ///< The queues for each event type.
            World& world;                                  ///< The world feeding this bus.
        };
    }  // namespace Event
}  // namespace rome::core