#pragma once

#include "rm/container/bitset.hpp"
#include "rm/ecs/world.hpp"

namespace rome::core {
    namespace System {
        struct Context;

        /**
         * @brief A system's descriptor defines its static properties such as name, scheduling, etc.
         */
        struct Descriptor {
            World& world;                                  ///< Reference to the world instance.
            const std::string name = "null descriptor";    ///< The name of the system. Must be unique.
            std::function<void(Context&)> callback;        ///< The function to be called every time the system is executed.
            BitSet<> reads;                                ///< The components this system reads.
            BitSet<> writes;                               ///< The components this system writes.
            BitSet<> emits;                                ///< The events this system emits.
            BitSet<> listens;                              ///< The events this system listens to.
            b8 requireFull = false;                        ///< Whether the system must operate on a full-owning group.
            b8 allowPartial = false;                       ///< Whether the system can operate on partial groups.
            b8 active = true;                              ///< Whether the system is currently active.
        };

        class Builder {
            public:
            Builder(const std::string& name, World& world);
            ~Builder() = default;
            Builder(const Builder&) = delete;
            Builder& operator=(const Builder&) = delete;
            Builder(Builder&&) = delete;
            Builder& operator=(Builder&&) = delete;

            /**
             * @brief Sets the name of the system.
             * @param name The name to set.
             * @return This builder instance for chaining.
             */
            template <Component::Component... Args>
            Builder& reads() {
                descriptor.reads = BitSet<>::create<Component::ID>({submitComponent<Args>()...});
                return *this;
            }

            /**
             * @brief Sets the components this system writes to.
             * @tparam Args The component types to write to.
             * @return This builder instance for chaining.
             */
            template <Component::Component... Args>
            Builder& writes() {
                descriptor.writes = BitSet<>::create<Component::ID>({submitComponent<Args>()...});
                return *this;
            }

            /**
             * @brief Sets the events this system emits.
             * @param events The events to emit.
             * @return This builder instance for chaining.
             */
            Builder& emits(std::initializer_list<Event::ID> events);

            /**
             * @brief Sets the events this system listens to.
             * @param events The events to listen to.
             * @return This builder instance for chaining.
             */
            Builder& listens(std::initializer_list<Event::ID> events);

            /**
             * @brief Requires the system to operate on a full-owning group.
             * @return This builder instance for chaining.
             */
            Builder& requireFull();

            /**
             * @brief Allows the system to operate on partial groups.
             * @return This builder instance for chaining.
             */
            Builder& allowPartial();

            /**
             * @brief Creates a ready-to-submit system descriptor.
             * @tparam Lambda The type of the user‑supplied lambda or functor.
             * @param callback The user‑supplied lambda or functor.
             * @return A fully‑populated Descriptor.
             */
            template <typename Lambda>
            Descriptor&& build(Lambda&& callback) {
                STATIC_ASSERT(std::is_invocable_v<Lambda&, Context&>, "Callback must be callable with (System::Context&)");

                descriptor.callback = std::forward<Lambda>(callback);
                return std::move(descriptor);
            }

            private:
            template <Component::Component T>
            Component::ID submitComponent() {
                world.components.submit<T>();
                return world.components.getID<T>();
            }

            Descriptor descriptor;  ///< The system's descriptor being built.
            World& world;           ///< Reference to the world instance.
        };
    }  // namespace System
}  // namespace rome::core
