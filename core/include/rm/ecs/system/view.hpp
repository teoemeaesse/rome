#pragma once

#include "rm/ecs/group/group.hpp"
#include "rm/ecs/world.hpp"

namespace rome::core {
    namespace System {
        // Helpers to find the index of a type in a parameter pack.
        template <typename T, typename... List>
        struct index_of;
        template <typename T, typename First, typename... Rest>
        struct index_of<T, First, Rest...> {
            static constexpr u64 value = [] {
                if constexpr (std::is_same_v<T, First>)
                    return 0;
                else
                    return 1 + index_of<T, Rest...>::value;
            }();
        };
        template <typename T>
        struct index_of<T> {
            STATIC_ASSERT(sizeof(T) == 0, "Type not found in index_of");
        };

        struct RM_API Context {
            const Group& group;  ///< The group this system is operating on.
            World& world;        ///< Reference to the world instance.
        };

        template <Component::Component... Components>
        class RM_API ViewIterator final {
            public:
            ViewIterator(const std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...>& pools, const Entity* entities, u64 index,
                         u64 max)
                : pools(pools), entities(entities), index(index), max(max) {}

            bool operator!=(const ViewIterator& iter) const { return index != iter.index; }
            ViewIterator& operator++() {
                index++;
                return *this;
            }

            decltype(auto) operator*() const {
                return std::apply(
                    [this](auto*... pool) { return std::forward_as_tuple(fetch<Components>(pool, entities[index])...); },
                    pools);
            }

            private:
            const std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...> pools;  ///< Pointers to component pools.
            const Entity* entities;                                                            ///< Pointer to the entities.
            const u64 max;                                                                     ///< Maximum number of entities in the view.
            u64 index;                                                                         ///< Current index in the view.

            /**
             * @brief Fetches the component for a given entity. Decides whether to use the owned pointer or the pool based on availability.
             * @tparam T The component type to fetch.
             * @param owned Pointer to the owned component, if any.
             * @param pool Pointer to the component pool, if not owned.
             * @param e The entity to fetch the component for.
             * @return A reference to the component.
             */
            template <class T>
            static decltype(auto) fetch(auto* pool, Entity e) {
                auto* component = pool->get(e);
                RM_ASSERT_MSG(component != nullptr, "Entity is missing a component required by the view");

                if constexpr (std::is_const_v<T>)
                    return static_cast<const remove_all_qualifiers_t<T>&>(*component);
                else
                    return static_cast<remove_all_qualifiers_t<T>&>(*component);
            }
        };

        /**
         * @brief A view over a set of components for a specific group of entities.
         * @tparam ...Components The component types to include in the view.
         */
        template <Component::Component... Components>
        class RM_API View final {
            public:
            explicit View(Context& ctx) : count(ctx.group.getSize()) {
                auto [entities, _] = ctx.group.getEntities().getData();
                this->entities = entities;

                (void)std::initializer_list<int>{(source<Components>(ctx), 0)...};
            }

            ViewIterator<Components...> begin() const { return ViewIterator<Components...>{pools, entities, 0, count}; }
            ViewIterator<Components...> end() const { return ViewIterator<Components...>{pools, entities, count, count}; }

            private:
            template <Component::Component T>
            void source(Context& ctx) {
                constexpr u64 index = index_of<remove_all_qualifiers_t<T>, remove_all_qualifiers_t<Components>...>::value;
                auto* pool = ctx.world.components.getPool<remove_all_qualifiers_t<T>>();
                RM_ASSERT_MSG(pool != nullptr, "View requested an unregistered component pool");
                std::get<index>(pools) = pool;
            }

            std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...> pools;  ///< Pointers to component pools.
            const Entity* entities;                                                      ///< Pointer to the entities.
            const u64 count;                                                             ///< Number of entities in the view.
        };
    }  // namespace System
}  // namespace rome::core
