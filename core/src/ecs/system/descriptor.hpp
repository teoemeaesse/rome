#pragma once

#include "container/bitset.hpp"
#include "ecs/world.hpp"

namespace rome::core {
    namespace System {
        struct Context;

        /**
         * @brief A system's descriptor defines its static properties such as name, scheduling, etc.
         */
        struct RM_API Descriptor {
            const World& world;                            ///< Reference to the world instance.
            const std::string name = "null descriptor";    ///< The name of the system. Must be unique.
            const std::function<void(Context&)> callback;  ///< The function to be called every time the system is executed.
            BitSet<Component::ID> reads;                   ///< The components this system reads.
            BitSet<Component::ID> writes;                  ///< The components this system writes.
            BitSet<Event::ID> emits;                       ///< The events this system emits.
            BitSet<Event::ID> listens;                     ///< The events this system listens to.
            b8 requireFull = true;                         ///< Whether the system must operate on a full-owning group.
            b8 allowPartial = false;                       ///< Whether the system can operate on partial groups.
            b8 active = true;                              ///< Whether the system is currently active.
        };

        class RM_API Builder {
            public:
            Builder(const std::string& name, const World& world);
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
                descriptor.reads({world.components.enter<Args>()...});
                return *this;
            }

            /**
             * @brief Sets the components this system writes to.
             * @tparam Args The component types to write to.
             * @return This builder instance for chaining.
             */
            template <Component::Component... Args>
            Builder& writes() {
                descriptor.writes({world.components.enter<Args>()...});
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
             * @brief Sets whether the system must operate on a full-owning group.
             * @param value Whether to require full ownership.
             * @return This builder instance for chaining.
             */
            Builder& requireFull(b8 value = true);

            /**
             * @brief Sets whether the system can operate on partial groups.
             * @param value Whether to allow partial groups.
             * @return This builder instance for chaining.
             */
            Builder& allowPartial(b8 value = true);

            /**
             * @brief Creates a ready-to-register system descriptor.
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
            Descriptor descriptor;  ///< The system's descriptor being built.
            const World& world;     ///< Reference to the world instance.
        };
    }  // namespace System
}  // namespace rome::core